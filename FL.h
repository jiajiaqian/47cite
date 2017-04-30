#ifndef   FL_H_FILE       //���û�ж��������   
#define  FL_H_FILE       //���������
#include <windows.h>
#include <atltime.h>
using namespace std;

#ifdef	SSLAPI
#else
#define	SSLAPI	extern "C" __declspec(dllimport)
#endif

struct ThreadDefine//�̶߳���
{
	short ThreadID;//�߳�ID��
	char  ThreadName[32];//�߳���
	DWORD RequestWord;//����������
	int   ThreadTN;//�̴߳�������
	short  status;//0---ֹͣ��1--���У�2--������
	short  SimFlag;//ģ���־λ
	HANDLE ThreadHandle;//�߳̾��
	HANDLE ThreadEvent[2];//�߳��¼�[0]---�������߳��¼�;[1]---�رո��߳��¼�
	int   CurrentUseCameraNum;//��ǰ������洢����
	int   OldUseCameraNum;//��һ��������洢����
	int   CameraNum1;//��ǰ1�������
	int   CameraNum2;//��ǰ2�������
	int   TimedCameraNum1;//��ǰ1�������
	int   TimedCameraNum2;//��ǰ2�������
	int   isCamera1FileRecord;
	int   isCamera2FileRecord;
	int   ThreadStructSize;//�߳̽ṹ���С
};
struct LogUnitDefine//��־��Ԫ����
{
	long Index;//��־����
	char LogTime[14];//��¼time
	LARGE_INTEGER  AbsoluteTimeOffset;//����timeƫ����8�ֽ�
	short LogLevel;//��־����
	short LogThreadID;//�̺߳�
	char  Log[64];//��־����;
};
struct LogThreadDefine//�߳���־����
{
	short ThreadID;//�̺߳�
//	int   LogNumOld;//��һ�μ�¼��
	int   LogNum;//��¼��־��
	int   TimedLogNum;//��ǵ���־����
	int   isFileRecord;//�Ƿ��Ѿ���¼
	LogUnitDefine unit[2048];
};


enum{
	TYPE_SOCKETLINE=3, TYPE_S7LINE, TYPE_DBLINE, TYPE_TRACK, TYPE_UFCMODEL, TYPE_UFCSIGNALPROCESS, TYPE_UFCDATARECORD
}enumThreadType;

enum{
	STATE_RUN=0, STATE_STOP
}enumThreadState;

struct SysDefine
{
	char ProjectName[32];
	short ThreadNum;//�߳���
	ThreadDefine thread[64];//64���߳�
	char saveDir[256];//�洢Ŀ¼
	short CurrentUseLogBuffer[64];//��ǰʹ�û�����
	short OldUseLogBuffer[64];//��ǰʹ�û�����
	LogThreadDefine LogBuffer1[64];//�̻߳�����1
	LogThreadDefine LogBuffer2[64];//�̻߳�����2
	char AppStruct[64][1024*1024];
	char Camera1[64][1024*1024];
	char Camera2[64][1024*1024];
};

typedef unsigned (__stdcall *PTHREAD_START)(void *); 

SSLAPI bool LogWrite(int ThreadID,int LogLevel,char *Log,int LogLength);//��¼��־
// �ӿں���
SSLAPI HANDLE beginthread(int idThread,PTHREAD_START ThreadStartAddr);//�����߳�
//SSLAPI void endthread(HANDLE th);//��ֹ�߳�********����ͨ�����߳��Լ�return���������������endthread��������Ҫ�ر��߳̾������closehandle()
SSLAPI bool setTRB(int idThread, int taskNo);//�趨����������
SSLAPI bool resetTRB(int idThread, int taskNo);
SSLAPI bool setTRBs(int idThread, DWORD rBits);//ͬʱ�趨�������������
SSLAPI DWORD getTRW(int idThread);//�������������
SSLAPI unsigned int lockedGetRW(unsigned int* rw);//����ȡ������״̬��
SSLAPI bool lockedSetRW(unsigned int* rw, int BitNumber);//������������״̬��
SSLAPI bool testRW(unsigned int* rw, int BitNumber);//��������״̬��
SSLAPI void setSIMFlag(int ThreadID,bool SIMFlag);//����ģ���־��,��ThreadID=0ʱ��ʾ�����߳�
SSLAPI bool getSIMFlag(int ThreadID);//���ģ���־��
SSLAPI void setSaveDir(char* pAppDir);//�洢����Ŀ¼
SSLAPI void getSaveDir(char* pAppDir);//��ó���Ŀ¼
SSLAPI bool createShareMemory(int MemorySize,char *MemoryName);//����ϵͳ�����ڴ�
SSLAPI bool initSysService();//��ʼ��ϵͳ���񣬴򿪾���time��ͼ�¼�ļ��߳�
SSLAPI bool startSysService();//��ʼ��ϵͳ���񣬴򿪾���time��ͼ�¼�ļ��߳�
SSLAPI void closeShareMemory();//�ر�ϵͳ�����ڴ�
SSLAPI bool createThreadEvent();//��ʼ��ϵͳ�����ڴ�
SSLAPI bool closeThreadEvent();//�ر�ϵͳ�����ڴ�
SSLAPI bool SetCameraSize(int ThreadID,int BufferSize);//���������
SSLAPI bool Camera(int ThreadID,byte *StartAddress);//���������
SSLAPI void lockedmemcpy(void* to, void* from, size_t count);
SSLAPI bool ExitRecordProcess();//�˳�����

#endif