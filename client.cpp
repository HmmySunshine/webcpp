#include "client.h"


Client::Client() : 
	clntSock(INVALID_SOCKET)
{

}

Client::Client(SOCKET clntSock_)
	: clntSock(clntSock_)
{

}

SOCKET Client::getSocket() const
{
	return this->clntSock;
}

void Client::setSecondBufLen(unsigned secondBufLen_)
{
	secondBufLen = secondBufLen_;
}

char* Client::getSecondBuf()
{
	return secondBuf;
}


unsigned Client::getSecondBufLen() const
{
	return secondBufLen;
}