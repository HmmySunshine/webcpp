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
//ip����ʹ��c++stirng
bool Server::Init(const char* ip, int port)
{
	//��windows�����
	WSADATA data{};
	if (WSAStartup(MAKEWORD(2, 2), &data) == SOCKET_ERROR)
	{
		std::cout << "WSAStartup error,error num: " << WSAGetLastError() << std::endl;
		return false;
	}
	//����socket
	servSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (servSock == INVALID_SOCKET)
	{
		std::cout << "socket error, error num:" << WSAGetLastError() << std::endl;
		WSACleanup();
		return false;
	}
	//��ip�Ͷ˿�
	sockaddr_in serAddr{};
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(2345);
	
	inet_pton(AF_INET, ip, &serAddr.sin_addr.S_un.S_addr);
	//���׽���
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


// �رշ�����
void Server::Close()
{
	// ����������׽��ֲ�Ϊ��Чֵ
	if (servSock != INVALID_SOCKET)
	{
		// �رշ������׽���
		closesocket(servSock);
		// ���÷������׽���Ϊ��Чֵ
		servSock = INVALID_SOCKET;
	}
	for (auto recvServer : recvServerVec)
		delete recvServer;
	// ���������׽���
	WSACleanup();
	
}

// ���ͻ�����ӵ����շ�����
void Server::AddClientToRecvServer(Client* pClient)
{
	// ��ȡ���շ����������е���С�ͻ�������
	RecvServer* pMinRecvServer = recvServerVec[0];
	for (USHORT i = 1; i < RECV_SERVE_SIZE; ++i)
	{
		if (pMinRecvServer->getClientNum() > recvServerVec[i]->getClientNum())
			pMinRecvServer = recvServerVec[i];
	}
	// ���ͻ�����ӵ���С�ͻ��������Ľ��շ���������Ϣ������
	pMinRecvServer->AddClientToBufVec(pClient);
}

void Server::recvMessage()
{
	std::cout << "recv message" << std::endl;
}



bool Server::CoreFunc()
{
	//���� servSock ������ INVALID_SOCKET��
	assert(servSock != INVALID_SOCKET);
	//����׼��д��while(1)֮����Ż�
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

