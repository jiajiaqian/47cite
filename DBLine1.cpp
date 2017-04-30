/****************************************************************************/

#include "stdafx.h"
#include "SAPI_DB.h"
#include "PCFramework.h"
#include "ADO/adodata.h"
#include "MainFrm.h"

unsigned __stdcall DBLine1(void* pv)
{
	//#define SQL_IP_ADCOS   "10.30.31.51"
	#define ORACLE_IP   "ng4700"
	#define ORACLE_NAME_ADCOS "ng4700l2"
	#define ORACLE_PASS_ADCOS "think"

	CPCFrameWorkApp * pApp;
	pApp=(CPCFrameWorkApp *)AfxGetApp(); 
	UFCDefine *ufcs=pApp->ufcs; 
	S7Line1Define *s71=pApp->s71;

	bool bIsOK = false;
	ADODATA * AdcosAdoData = new ADODATA();
	CString str,str_oracle; 
	CString vSQL;

	//和下面的数据连接调换位置，保证弹出界面不受数据库连接超时影响
	SetEvent(pApp->L2Sys->thread[TYPE_DBLINE].ThreadEvent[STATE_RUN]);
	Sleep(200);
	WaitForSingleObject(pApp->L2Sys->thread[TYPE_DBLINE].ThreadEvent[STATE_RUN], INFINITE);

	//如果COM对象初始化失败，则不进行下面的操作
	if(AdcosAdoData->InitCom())
	{
		bIsOK = AdcosAdoData->SetConnection(ORACLE_IP,"",ORACLE_NAME_ADCOS,ORACLE_PASS_ADCOS,"ORACLE");  // 此处可能有问题 [9/13/2013 谢谦]
		logger::LogDebug("db", "数据库连接成功");
	}
	else{
		logger::LogDebug("db", "数据库连接失败");
	}

	unsigned int req;
	int nIndex = 0;
	while (true) 
	{
		nIndex=WaitForMultipleObjects(2,pApp->L2Sys->thread[TYPE_DBLINE].ThreadEvent, FALSE,INFINITE);
		if(nIndex==WAIT_OBJECT_0+1)
		{
			delete AdcosAdoData;
			AdcosAdoData = NULL;
			SetEvent(pApp->L2Sys->thread[TYPE_DBLINE].ThreadEvent[STATE_STOP]);
			break;
		}
		else if(nIndex==WAIT_OBJECT_0)
		{
			try
			{
				lockedmemcpy(&req,&(pApp->L2Sys->thread[TYPE_DBLINE].RequestWord),4);
				for(int i = 0; i < 32; i++)
				{
					if(testRW(&req, i))
					{
						switch(i)
						{
						case 0://调用存储过程 add by Zed 2013.11
							if(bIsOK)
							{	
								//if(!AdcosAdoData->ExecGetRedTemp(&DB))
								//{
								//	logger::LogDebug("db", "DBLine1.cpp----------ERROR!数据库操作没有成功执行");
								//}
								//else{
								//	logger::LogDebug("db", "DBLine1.cpp----------OK,成功存储到数据库");
								//}
							}else{
								logger::LogDebug("db", "DBLine1.cpp----------ERROR!,数据库没有打开");
							}
							break;
						case 1://执行数据库add by Zed 2013.11
							if(bIsOK)
							{								
								if(!AdcosAdoData->Execute(ufcs->CSpdiAdcos))
								{
									logger::LogDebug("db", "DBLine1.cpp----------ERROR!数据库操作没有成功执行");
								}
								else{
									logger::LogDebug("db", "DBLine1.cpp----------OK,成功存储到数据库");
								}
							}else{
								logger::LogDebug("db", "DBLine1.cpp----------ERROR!,数据库没有打开");
							}
							break;
						default:
							break;
						}
						resetTRB(4,i);
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
