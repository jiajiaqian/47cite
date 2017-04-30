#include "stdafx.h"
#include "PCFramework.h"
#include "S7Line1.h"

S7Line  s7;

boost::atomic<bool> gbS7ThreadSwitch= true;
boost::pool<> gPoolS7Recv(sizeof(S7Line1R1Define)*10);
boost::lockfree::queue<S7Line1R1Define *> gS7LineQueue(10);
EventNotify gS7RecvEvent;
boost::thread_group gS7Threads;

void RecvS7Thread();

void S7RecvProcess(int nType, const char* pRecv, int nSize);

/// @函数说明 S7监控线程
/// 返回value 返回说明
/// 作者 Hanb
/// 日期 8/6/2013
unsigned __stdcall S7Line1(void* pv)
{
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 

	SetEvent(pApp->L2Sys->thread[TYPE_S7LINE].ThreadEvent[STATE_RUN]);
	Sleep(200);
	WaitForSingleObject(pApp->L2Sys->thread[TYPE_S7LINE].ThreadEvent[STATE_RUN], INFINITE);

	s7.S7CreateLine(1);
	Sleep(1000);
	s7.S7SetRecieveBufferFlag(1,(char *)pApp->s71->RecieveBufferFlag);
	s7.S7SetRecieveProcessFlag(1,(char *)pApp->s71->RecieveProcessFlag);	
	s7.S7SetRecieveMessageMemory(1,1,(char *)(&(pApp->s71->r1[0])));
	s7.S7SetRecieveMessageTriggerEvent(1,1,"UFCSignalProcess");
	s7.S7SetSendBufferFlag(1,(char *)pApp->s71->SendBufferFlag);
	s7.S7SetSendProcessFlag(1,(char *)pApp->s71->SendProcessFlag);
	strcpy(pApp->s71->s1[0].Trk_ID,"KKKK");
//	s7.S7SetSendMessageMemory(1,1,(char *)(pApp->s71->s1));
//	s7.S7SetSendMessageMemory(1,2,(char *)(pApp->s71->s2));
//	s7.S7SetSendMessageMemory(1,3,(char *)(pApp->s71->s3));

	s7.S7SetRecieveCallbk(S7RecvProcess);
	gS7Threads.create_thread(RecvS7Thread);

	s7.S7Start(1);
	Sleep(500);
	
	unsigned int req;
	int nIndex;
	while (1) 
	{
		nIndex=WaitForMultipleObjects(2,pApp->L2Sys->thread[TYPE_S7LINE].ThreadEvent, FALSE,INFINITE);
		if(nIndex==WAIT_OBJECT_0+1)
		{
			s7.S7Stop(1);
			s7.S7DeleteLine(1);
			SetEvent(pApp->L2Sys->thread[TYPE_S7LINE].ThreadEvent[STATE_STOP]);
			gbS7ThreadSwitch = false;
			gS7RecvEvent.Notify();
			gS7Threads.join_all();
			break;
		}
		else if(nIndex==WAIT_OBJECT_0)
		{
			try
			{
				lockedmemcpy(&req,&(pApp->L2Sys->thread[TYPE_S7LINE].RequestWord),4);
				for(int i = 0; i < 32; i++)
				{
					if(testRW(&req, i))
					{
						switch(i)
						{
						case 0://L2TOPLCID1
							if(pApp->s71->SendBufferFlag[0]>=31)
								pApp->s71->SendBufferFlag[0]=1;				
							else
								pApp->s71->SendBufferFlag[0]++;
							
							//memcpy((char *)(&pApp->s71->s1[0])+424,(char *)(&pApp->s71->s1),424); // old [9/27/2013 谢谦]
//							memcpy(&pApp->s71->s1[pApp->s71->SendBufferFlag[0]],&pApp->s71->s1[0],sizeof(S7Line1S1Define));  // 参考s2 [9/27/2013 谢谦]
//							s7.S7SendMessage(1,1);
							s7.S7SendMessage(1,1, (char*)&pApp->s71->s1[0], sizeof(S7Line1S1Define));
							break;
						case 1://L2TOPLCID2 
							if(pApp->s71->SendBufferFlag[1]>=31)
								pApp->s71->SendBufferFlag[1]=1;				
							else
								pApp->s71->SendBufferFlag[1]++;
							memcpy(&pApp->s71->s2[pApp->s71->SendBufferFlag[1]],&pApp->s71->s2[0],sizeof(S7Line1S2Define));
							s7.S7SendMessage(1,2, (char*)&pApp->s71->s2[0],sizeof(S7Line1S2Define));
							break;
						case 2://L2TOPLCID3
							if(pApp->s71->SendBufferFlag[2]>=31)
								pApp->s71->SendBufferFlag[2]=1;				
							else
								pApp->s71->SendBufferFlag[2]++;
							memcpy(&pApp->s71->s3[pApp->s71->SendBufferFlag[2]],&pApp->s71->s3[0],sizeof(S7Line1S3Define));
							s7.S7SendMessage(1,3, (char*)&pApp->s71->s3[0],sizeof(S7Line1S3Define));
							break;
						case 31://
							s7.S7ShowConfig(1);
							break;
						default:
							break;
						}
						resetTRB(5,i);
					}
				}

			}
			catch(...)
			{
				}
		}
	
	}
	return 0;
}

void NotifyS7()
{
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 

	if(pApp->s71->SendBufferFlag[0]>=31)
		pApp->s71->SendBufferFlag[0]=1;				
	else
		pApp->s71->SendBufferFlag[0]++;

	memcpy((char *)(&pApp->s71->s1[0])+424,(char *)(&pApp->s71->s1),424);
	//s7.S7SendMessage(1,1);
}


void RecvS7Thread()
{
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 

	while(gbS7ThreadSwitch){
		gS7RecvEvent.Wait();
		PGfcEXTL2PdiData pData;
		while(gS7LineQueue.pop(pData)){
			CPCFrameWorkApp *theApp=(CPCFrameWorkApp *)AfxGetApp();

			memcpy(&pApp->s71->r1ontime, pData, sizeof(S7Line1R1Define));
			s7Track();
			gPoolS7Recv.free(pData);
		}
	}
}

void S7RecvProcess(int nType, const char* pRecv, int nSize)
{
	if(nType == 0){
		S7Line1R1Define *pData = (S7Line1R1Define *)gPoolS7Recv.malloc();
		memcpy(pData, pRecv, sizeof(S7Line1R1Define));
		gS7LineQueue.push(pData);
		gS7RecvEvent.Notify();
	}
}

