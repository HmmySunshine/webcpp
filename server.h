#pragma once
#include "recvserver.h"

const unsigned short RECV_SERVE_SIZE = 4;
class Server : public MessageInterface
{
private:
	SOCKET servSock;
	bool CoreFunc();
	std::vector<RecvServer*> recvServerVec;
	
public:
	Server();
	~Server();
	bool Start();
	bool Init(const char* ip, int port);
	void Close();
	void AddClientToRecvServer(Client* pClient);
	void recvMessage();
	
};