#pragma once
#include "client.h"
#include "Message.hpp"
#include <vector>
#include <map>
#include <mutex>
#include "MessageInterface.h"
class RecvServer{
public:
	RecvServer(MessageInterface* pMsgInterface_);
	~RecvServer();
	void Start();
	void Close();
	void AddClientToBufVec(Client* pClient);
	unsigned getClientNum();
private:
	bool RecvMsg(Client* pClient);
	bool ConductMsg(Header* pHeader, Client* pClient);
	void CoreFunc();
	
	//客户缓冲区
	std::vector<Client*> clientVec;
	std::map<SOCKET, Client*> clientMap;
	//第一缓冲区
	char ftBuf[MSG_BUF_SIZE]{};
	std::mutex _mutex;
	MessageInterface* pMsgInterface;

};