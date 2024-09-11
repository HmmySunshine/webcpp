#include "server.h"
#include <iostream>
#include <WS2tcpip.h>
#include <assert.h>

bool Server::Start()
{
	for (USHORT i = 0; i < RECV_SERVE_SIZE; ++i)
	{
		RecvServer* recvServer = new RecvServer(this);
		recvServer->Start();
		recvServerVec.push_back(recvServer);
	}
	if (!CoreFunc())
	{
		return false;
	}
	return true;
	
}
Server::Server() : servSock(INVALID_SOCKET)
{
}
Server::~Server()
{
	Close();
}
//ip后续使用c++stirng
bool Server::Init(const char* ip, int port)
{
	//打开windows网络库
	WSADATA data{};
	if (WSAStartup(MAKEWORD(2, 2), &data) == SOCKET_ERROR)
	{
		std::cout << "WSAStartup error,error num: " << WSAGetLastError() << std::endl;
		return false;
	}
	//创建socket
	servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (servSock == INVALID_SOCKET)
	{
		std::cout << "socket error, error num:" << WSAGetLastError() << std::endl;
		WSACleanup();
		return false;
	}
	//绑定ip和端口
	sockaddr_in serAddr{};
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(2345);
	
	inet_pton(AF_INET, ip, &serAddr.sin_addr.S_un.S_addr);
	//绑定套接字
	if (bind(servSock, (sockaddr*)&serAddr, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		std::cout << "bind error,error num:" << WSAGetLastError() << std::endl;
		closesocket(servSock);
		servSock = INVALID_SOCKET;
		WSACleanup();
		return false;
	}
	if (listen(servSock, 128) == SOCKET_ERROR)
	{
		std::cout << "listen error,error num:" << WSAGetLastError() << std::endl;
		closesocket(servSock);
		servSock = INVALID_SOCKET;
		WSACleanup();

		return false;
	}
	return true;
}


// 关闭服务器
void Server::Close()
{
	// 如果服务器套接字不为无效值
	if (servSock != INVALID_SOCKET)
	{
		// 关闭服务器套接字
		closesocket(servSock);
		// 设置服务器套接字为无效值
		servSock = INVALID_SOCKET;
	}
	for (auto recvServer : recvServerVec)
		delete recvServer;
	// 清理所有套接字
	WSACleanup();
	
}

// 将客户端添加到接收服务器
void Server::AddClientToRecvServer(Client* pClient)
{
	// 获取接收服务器队列中的最小客户端数量
	RecvServer* pMinRecvServer = recvServerVec[0];
	for (USHORT i = 1; i < RECV_SERVE_SIZE; ++i)
	{
		if (pMinRecvServer->getClientNum() > recvServerVec[i]->getClientNum())
			pMinRecvServer = recvServerVec[i];
	}
	// 将客户端添加到最小客户端数量的接收服务器的消息缓冲区
	pMinRecvServer->AddClientToBufVec(pClient);
}

void Server::recvMessage()
{
	std::cout << "recv message" << std::endl;
}



bool Server::CoreFunc()
{
	//断言 servSock 不等于 INVALID_SOCKET。
	assert(servSock != INVALID_SOCKET);
	//不标准的写法while(1)之后会优化
	while (1)
	{
		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(servSock, &fdRead);
		timeval timeout{ 2, 0 };
		int selectRet = select(0, &fdRead, nullptr, nullptr, &timeout);
		if (selectRet > 0)
		{
			for (unsigned int i = 0; i < fdRead.fd_count; ++i)
			{
				
				SOCKET clntSock = accept(servSock, nullptr, nullptr);
				if (clntSock == INVALID_SOCKET)
				{
					std::cout << "accept error,error num:" << WSAGetLastError() << std::endl;
					break;
				}
				
				AddClientToRecvServer(new Client(clntSock));
			}
		}
		else if (selectRet == 0)
		{
			std::cout << "select func no client connetcted do something else" << std::endl;
			continue;
		}
		else
		{
			std::cout << "select func error , error num :" << WSAGetLastError() << std::endl;
			//break;
			return false;
		}
	}
	return true;
}

