#ifndef   DB_H_FILE       //如果没有定义这个宏   
#define  DB_H_FILE       //定义这个宏
#ifdef	SAPI_DB
#else
#define	SAPI_DB	extern "C" __declspec(dllimport)
#endif
#include "BaseCoolStruct.h"

struct ColumDefine//线程定义
{
	char colum_string[32];
	int  colum_type;//类型,1-unsigned short
						 //2-unsigned int
						 //3-short
						 //4-int
						 //5-float
						 //6-double
						 //7-char
};
struct CommandDefine//线程定义
{
	int  Index;//索引号
	int  OperateFlag;//操作代码0---写入，1---读取
	char TableName[32];
	char SelectString[32];//删除、选择、更新的时候
	int  ColumNum;//列数
	ColumDefine Colum[64];//可以有这么多个列
	char CommandString[256];
	char *InputValue;//输入value
	char *OutBuffer;//返回value ，如果有返回value的话
};

class _declspec(dllimport) DBLine
{
public:
	DBLine();

	~DBLine();
public:
	bool excuteCommand(int LineIndex,int CommandIndex);//执行索引号
	bool setCommand(int LineIndex,int CommandIndex,int Index,CommandDefine *Commands);//
	bool setValue(int LineIndex,int CommandIndex,int direct,char *Buffer);//
	void setReconnectFlag(int LineIndex,bool Flag);//
	bool connectDB(int LineIndex);
	bool disconnectDB(int LineIndex);
	bool setConnectionpara(int LineIndex,char *serverName,char *databaseName,char *user,char *passWord,char *db_type);
};
#endif