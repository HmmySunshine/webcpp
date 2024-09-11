#include "recvserver.h"
#include <iostream>
#include <format>
#include <thread>
RecvServer::RecvServer(MessageInterface* pMsgInterface_)
	: pMsgInterface(pMsgInterface_)
{
}

RecvServer::~RecvServer()
{
	Close();
}

void RecvServer::Start()
{
	std::thread myThread(&RecvServer::CoreFunc, this);
	myThread.detach();
}

void RecvServer::Close()
{
	if (!(clientVec.empty() && clientMap.empty()))
	{
		for (auto pClient : clientVec)
		{
			closesocket(pClient->getSocket());
			delete pClient;
		}
			
		for (auto pClientMapPair : clientMap)
		{
			closesocket(pClientMapPair.first);
			delete pClientMapPair.second;
		}
		clientVec.clear();
		clientMap.clear();
	}
}

void RecvServer::AddClientToBufVec(Client* pClient)
{
	_mutex.lock();
	clientVec.push_back(pClient);
	_mutex.unlock();
}

bool RecvServer::RecvMsg(Client* pClient)
{
	int recvRet = recv(pClient->getSocket(), ftBuf, MSG_BUF_SIZE, 0);
	pMsgInterface->recvMessage();

	if (recvRet <= 0)
	{
		int errorNum = WSAGetLastError();
		if (errorNum == 10054)
		{
			std::cout << std::format("客户端强制断开连接 error num:{}", errorNum) << std::endl;
		}
		std::cout << std::format("recv func error,error num:{}", errorNum) << std::endl;
		return false;
	}
	//复制第一缓冲区的内容到第二缓冲区 指针位置0 + 0  1024 - 0 
	memcpy_s(pClient->getSecondBuf() + pClient->getSecondBufLen(), MSG_BUF_SIZE * 10 - pClient->getSecondBufLen(),
		ftBuf, (unsigned)recvRet);
	unsigned bufLen = pClient->getSecondBufLen() + recvRet;
	pClient->setSecondBufLen(bufLen);
	while (pClient->getSecondBufLen() >= sizeof(Header))
	{
		Header* pHeader = (Header*)pClient->getSecondBuf();
		if (pClient->getSecondBufLen() >= pHeader->dataLength)
		{
			//数据包完整处理数据
			unsigned tempLen = pClient->getSecondBufLen() - pHeader->dataLength;

			if (!ConductMsg(pHeader, pClient)) return false;
			
			memcpy(pClient->getSecondBuf(), pClient->getSecondBuf() + pHeader->dataLength, tempLen);
			pClient->setSecondBufLen(tempLen);
		}
		else
		{
            break;
		}
	}
	return true;
}

bool RecvServer::ConductMsg(Header* pHeader, Client* pClient)
{
	switch (pHeader->cmd)
	{
	case CMD::LOGIN:
	{
		LoginResult loginResult{ 10 };
		if (send(pClient->getSocket(), (char*)&loginResult, sizeof(loginResult), 0) < 0)
		{
			std::cout << std::format("send func error,login error num:{}", WSAGetLastError()) << std::endl;
			return false;
		}
		break;
	}
	case CMD::LOGOUT:
	{
		LogoutResult logoutResult{ 10 };
		if (send(pClient->getSocket(), (char*)&logoutResult, sizeof(logoutResult), 0) < 0)
		{
			std::cout << std::format("send func error,logout error num:{}", WSAGetLastError()) << std::endl;
			return false;
		}
		break;
	}
	default:
		break;
	}
	return true;
}

void RecvServer::CoreFunc()
{
	while (1)
	{
		fd_set fdRead;
		FD_ZERO(&fdRead);
		{
			_mutex.lock();
			if (!clientVec.empty())
			{
				for (auto pClient : clientVec)
					clientMap.insert(std::make_pair(pClient->getSocket(), pClient));
			}
			_mutex.unlock();
		}
		for (auto clientMapPair : clientMap)
			FD_SET(clientMapPair.first, &fdRead);

		if (clientMap.empty()) continue;

		timeval timeout{ 2,0 };
		int selectRet =  select(0, &fdRead, nullptr, nullptr, &timeout);
		if (selectRet > 0)
		{
			for (unsigned i = 0; i < fdRead.fd_count; ++i)
			{		
				if (!RecvMsg(clientMap[fdRead.fd_array[i]]))
					std::cout << "recv func error" << std::endl;
			}
		}
		else if (selectRet == 0)
		{
			std::cout << "do something else no fRead Ready" << std::endl;
			continue;
		}
		else
		{
			std::cout << "select func error, error num:" << WSAGetLastError() << std::endl;
			return;
		}
	}
}

unsigned RecvServer::getClientNum()
{
	_mutex.lock();
	unsigned clientNum = clientVec.size() + clientMap.size();
	_mutex.unlock();
	return clientNum;
}
