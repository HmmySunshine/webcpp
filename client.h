#pragma once
#include <WinSock2.h>
#include "Message.hpp"
const unsigned MSG_BUF_SIZE = 1024;
class Client{
private:
	SOCKET clntSock;
	//第二缓冲区
	char secondBuf[MSG_BUF_SIZE * 10]{};
	//第二缓冲区数据长度
	unsigned secondBufLen = 0;
	
public:
	void setSecondBufLen(unsigned secondBufLen_);
	char* getSecondBuf();
	unsigned getSecondBufLen() const;
	Client();
	Client(SOCKET clnSock_);
	SOCKET getSocket() const;
};
