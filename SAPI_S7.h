
typedef void (* PS7RECVPROCESS)(int nType, const char* pBuf, int nSize);
class _declspec(dllimport) S7Line
{
public:
	S7Line();

	~S7Line();
public:
	bool S7CreateLine(int index);
	bool S7DeleteLine(int index);
	bool S7Start(int index);
	bool S7Stop(int index);
//    void S7SendMessage(int index,int message_index);
	void S7SendMessage(int index,int message_index,char *buffer,int data_length);

	void S7ShowConfig(int index);
	void S7SetRecieveBufferFlag(int line_index,char *buffer_index);
	void S7SetRecieveProcessFlag(int line_index,char *buffer_index);
	void S7SetRecieveMessageMemory(int index,int message_index,char *message_buffer);
	void S7SetRecieveMessageTriggerEvent(int index,int message_index,char *event_name);
	void S7SetSendBufferFlag(int line_index,char *buffer_index);
	void S7SetSendProcessFlag(int line_index,char *buffer_index);
	void S7SetSendMessageMemory(int index,int message_index,char *message_buffer);

	void S7SetRecieveCallbk(PS7RECVPROCESS pS7RecvProcess);
};
