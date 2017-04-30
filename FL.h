#ifndef   FL_H_FILE       //如果没有定义这个宏   
#define  FL_H_FILE       //定义这个宏
#include <windows.h>
#include <atltime.h>
using namespace std;

#ifdef	SSLAPI
#else
#define	SSLAPI	extern "C" __declspec(dllimport)
#endif

struct ThreadDefine//线程定义
{
	short ThreadID;//线程ID号
	char  ThreadName[32];//线程名
	DWORD RequestWord;//任务请求字
	int   ThreadTN;//线程触发次数
	short  status;//0---停止，1--运行，2--被触发
	short  SimFlag;//模拟标志位
	HANDLE ThreadHandle;//线程句柄
	HANDLE ThreadEvent[2];//线程事件[0]---启动该线程事件;[1]---关闭该线程事件
	int   CurrentUseCameraNum;//当前照相机存储区号
	int   OldUseCameraNum;//上一次照相机存储区号
	int   CameraNum1;//当前1照相机号
	int   CameraNum2;//当前2照相机号
	int   TimedCameraNum1;//当前1照相机号
	int   TimedCameraNum2;//当前2照相机号
	int   isCamera1FileRecord;
	int   isCamera2FileRecord;
	int   ThreadStructSize;//线程结构体大小
};
struct LogUnitDefine//日志单元定义
{
	long Index;//日志索引
	char LogTime[14];//记录time
	LARGE_INTEGER  AbsoluteTimeOffset;//绝对time偏移量8字节
	short LogLevel;//日志级别
	short LogThreadID;//线程号
	char  Log[64];//日志内容;
};
struct LogThreadDefine//线程日志定义
{
	short ThreadID;//线程号
//	int   LogNumOld;//上一次记录号
	int   LogNum;//记录日志号
	int   TimedLogNum;//标记的日志数号
	int   isFileRecord;//是否已经记录
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
	short ThreadNum;//线程数
	ThreadDefine thread[64];//64个线程
	char saveDir[256];//存储目录
	short CurrentUseLogBuffer[64];//当前使用缓冲区
	short OldUseLogBuffer[64];//当前使用缓冲区
	LogThreadDefine LogBuffer1[64];//线程缓冲区1
	LogThreadDefine LogBuffer2[64];//线程缓冲区2
	char AppStruct[64][1024*1024];
	char Camera1[64][1024*1024];
	char Camera2[64][1024*1024];
};

typedef unsigned (__stdcall *PTHREAD_START)(void *); 

SSLAPI bool LogWrite(int ThreadID,int LogLevel,char *Log,int LogLength);//记录日志
// 接口函数
SSLAPI HANDLE beginthread(int idThread,PTHREAD_START ThreadStartAddr);//启动线程
//SSLAPI void endthread(HANDLE th);//终止线程********我们通常让线程自己return，这种情况下无需endthread，但是需要关闭线程句柄，即closehandle()
SSLAPI bool setTRB(int idThread, int taskNo);//设定任务请求字
SSLAPI bool resetTRB(int idThread, int taskNo);
SSLAPI bool setTRBs(int idThread, DWORD rBits);//同时设定多个任务请求字
SSLAPI DWORD getTRW(int idThread);//获得任务请求字
SSLAPI unsigned int lockedGetRW(unsigned int* rw);//锁定取出任务状态字
SSLAPI bool lockedSetRW(unsigned int* rw, int BitNumber);//锁定设置任务状态字
SSLAPI bool testRW(unsigned int* rw, int BitNumber);//测试任务状态字
SSLAPI void setSIMFlag(int ThreadID,bool SIMFlag);//设置模拟标志字,当ThreadID=0时表示所有线程
SSLAPI bool getSIMFlag(int ThreadID);//获得模拟标志字
SSLAPI void setSaveDir(char* pAppDir);//存储程序目录
SSLAPI void getSaveDir(char* pAppDir);//获得程序目录
SSLAPI bool createShareMemory(int MemorySize,char *MemoryName);//创建系统共享内存
SSLAPI bool initSysService();//初始化系统服务，打开绝对time轴和记录文件线程
SSLAPI bool startSysService();//初始化系统服务，打开绝对time轴和记录文件线程
SSLAPI void closeShareMemory();//关闭系统共享内存
SSLAPI bool createThreadEvent();//初始化系统共享内存
SSLAPI bool closeThreadEvent();//关闭系统共享内存
SSLAPI bool SetCameraSize(int ThreadID,int BufferSize);//照相机功能
SSLAPI bool Camera(int ThreadID,byte *StartAddress);//照相机功能
SSLAPI void lockedmemcpy(void* to, void* from, size_t count);
SSLAPI bool ExitRecordProcess();//退出处理

#endif