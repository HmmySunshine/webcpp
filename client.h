#pragma once
#include <WinSock2.h>
#include "Message.hpp"
const unsigned MSG_BUF_SIZE = 1024;
class Client{
private:
	SOCKET clntSock;
	//�ڶ�������
	char secondBuf[MSG_BUF_SIZE * 10]{};
	//�ڶ����������ݳ���
	unsigned secondBufLen = 0;
	
public:
	void setSecondBufLen(unsigned secondBufLen_);
	char* getSecondBuf();
	unsigned getSecondBufLen() const;
	Client();
	Client(SOCKET clnSock_);
	SOCKET getSocket() const;
};
