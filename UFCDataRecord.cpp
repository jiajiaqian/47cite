#include "stdafx.h"
#include "PCFramework.h"

unsigned __stdcall UFCDataRecord(void* pv)
{
	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 

//	SetEvent(pApp->L2Sys->thread[TYPE_UFCDATARECORD].ThreadEvent[STATE_RUN]);
//	Sleep(200);
//	WaitForSingleObject(pApp->L2Sys->thread[TYPE_UFCDATARECORD].ThreadEvent[STATE_RUN], INFINITE);
	unsigned int req;
	int nIndex;
	while (1) 
	{
		nIndex=WaitForMultipleObjects(2,pApp->L2Sys->thread[TYPE_UFCDATARECORD].ThreadEvent, FALSE,INFINITE);
		if(nIndex==WAIT_OBJECT_0+1)	
		{
			SetEvent(pApp->L2Sys->thread[TYPE_UFCDATARECORD].ThreadEvent[STATE_STOP]);
			break;
		}
		else if(nIndex==WAIT_OBJECT_0)
		{
			try
			{
				lockedmemcpy(&req,&(pApp->L2Sys->thread[TYPE_UFCDATARECORD].RequestWord),4);
				for(int i = 0; i < 32; i++)
				{
					if(testRW(&req, i))
					{
						switch(i)
						{
						case 0://PLCTOL2ID1							
							break;
						case 1://SOCKETTOL2ID1												
							break;
						case 2://SOCKETTOL2ID2		
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



