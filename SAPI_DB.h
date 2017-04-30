#ifndef   DB_H_FILE       //���û�ж��������   
#define  DB_H_FILE       //���������
#ifdef	SAPI_DB
#else
#define	SAPI_DB	extern "C" __declspec(dllimport)
#endif
#include "BaseCoolStruct.h"

struct ColumDefine//�̶߳���
{
	char colum_string[32];
	int  colum_type;//����,1-unsigned short
						 //2-unsigned int
						 //3-short
						 //4-int
						 //5-float
						 //6-double
						 //7-char
};
struct CommandDefine//�̶߳���
{
	int  Index;//������
	int  OperateFlag;//��������0---д�룬1---��ȡ
	char TableName[32];
	char SelectString[32];//ɾ����ѡ�񡢸��µ�ʱ��
	int  ColumNum;//����
	ColumDefine Colum[64];//��������ô�����
	char CommandString[256];
	char *InputValue;//����value
	char *OutBuffer;//����value ������з���value�Ļ�
};

class _declspec(dllimport) DBLine
{
public:
	DBLine();

	~DBLine();
public:
	bool excuteCommand(int LineIndex,int CommandIndex);//ִ��������
	bool setCommand(int LineIndex,int CommandIndex,int Index,CommandDefine *Commands);//
	bool setValue(int LineIndex,int CommandIndex,int direct,char *Buffer);//
	void setReconnectFlag(int LineIndex,bool Flag);//
	bool connectDB(int LineIndex);
	bool disconnectDB(int LineIndex);
	bool setConnectionpara(int LineIndex,char *serverName,char *databaseName,char *user,char *passWord,char *db_type);
};
#endif