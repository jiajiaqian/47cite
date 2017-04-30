// Socket_API.h : Socket_API DLL 的主头文件
//
typedef void (* PSOCKETRECVPROCESS)(int nType, const char* pBuf, int nSize);

class _declspec(dllimport) SocketLine
{
public:
	SocketLine();

	~SocketLine();
public:
	bool SocketCreateLine(int index);
	bool SocketDeleteLine(int index);
	bool SocketStart(int index);
	bool SocketStop(int index);
    void SocketSendMessage(int index,int message_index);
	void SocketShowConfig(int index);
	void SocketLine::SocketSetRecieveBufferFlag(int line_index,char *buffer_index);
	void SocketLine::SocketSetRecieveProcessFlag(int line_index,char *buffer_index);
	void SocketLine::SocketSetRecieveMessageMemory(int index,int message_index,char *message_buffer);
	void SocketLine::SocketSetRecieveMessageTriggerEvent(int index,int message_index,char *event_name);
	void SocketLine::SocketSetSendBufferFlag(int line_index,char *buffer_index);
	void SocketLine::SocketSetSendProcessFlag(int line_index,char *buffer_index);
	void SocketLine::SocketSetSendMessageMemory(int index,int message_index,char *message_buffer);
	void SocketSetRecieveBufferIndex(int index,char *buffer_index);

	void SocketSetRecieveCallbk(PSOCKETRECVPROCESS pSocketRecvProcess);

};

