#include "stdafx.h"
#include "PCFramework.h"
unsigned __stdcall UFCModel(void* pv)
{
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 

//	SetEvent(pApp->L2Sys->thread[TYPE_UFCMODEL].ThreadEvent[STATE_RUN]);
//	Sleep(200);
//	WaitForSingleObject(pApp->L2Sys->thread[TYPE_UFCMODEL].ThreadEvent[STATE_RUN], INFINITE);
	unsigned int req;
	int nIndex;
	while (1) 
	{
		nIndex=WaitForMultipleObjects(2,pApp->L2Sys->thread[TYPE_UFCMODEL].ThreadEvent, FALSE,INFINITE);
		if(nIndex==WAIT_OBJECT_0+1)
		{
			SetEvent(pApp->L2Sys->thread[TYPE_UFCMODEL].ThreadEvent[STATE_STOP]);
			break;
		}
		else if(nIndex==WAIT_OBJECT_0)
		{
			try
			{
				lockedmemcpy(&req,&(pApp->L2Sys->thread[TYPE_UFCMODEL].RequestWord),4);
				for(int i = 0; i < 32; i++)
				{
					if(testRW(&req, i))
					{
						switch(i)
						{

						case 0://L2TOPLCID1
							//FCX_DATAPACK();//轧件进入炉后段
							//s7.S7SendMessage(1,1,(char *)&(UFCPLCCOM->m_L2ToPLCID1),50);
							break;
						case 1://PLCTOL2ID2
							//s7.S7SendMessage(1,2,(char *)&(UFCPLCCOM->m_L2ToPLCID2),50);
							break;
						case 31://
							break;
						default:
							break;
						}
						resetTRB(5,i);
					}
				}
			}
			catch(...){
			}
		}

	}
	return 0;
}