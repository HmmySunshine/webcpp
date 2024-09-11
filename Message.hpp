#pragma once
#include <cstring>
//��Ϣ״̬
enum class CMD
{
	LOGIN, //��¼
	LOGOUT, //�ǳ�
	LOGIN_RESULT, //��¼���
	LOGOUT_RESULT, //�ǳ����
	NO_CMD, //������
};

struct Header
{
	CMD cmd;
	unsigned dataLength;
	Header() :cmd(CMD::NO_CMD), dataLength(0)
	{
	}
	Header(CMD cmd_, unsigned datalength_) :cmd(cmd_), dataLength(datalength_)
	{

	}
};

struct Login : public Header
{
	char userName[32];
	char passWord[32];
	Login() :Header(CMD::LOGIN, sizeof(Login)), userName{ 0 }, passWord{ 0 }
	{

	}
	Login(const char* userName_, const char* passWord_)
		: Header(CMD::LOGIN, sizeof(Login))
	{
		strcpy_s(userName, 32, userName_);
		strcpy_s(passWord, 32, passWord_);
	}
};

struct Logout : public Header
{
	unsigned logoutNum;
	Logout() : Header(CMD::LOGOUT, sizeof(Logout)), logoutNum{ 0 }
	{

	}
	Logout(unsigned logoutNum_) : Header(CMD::LOGOUT, sizeof(Logout)), logoutNum(logoutNum_)
	{

	}
};

struct LoginResult : public Header
{
	unsigned loginResultNum;
	LoginResult() : Header(CMD::LOGIN_RESULT, sizeof(LoginResult)), loginResultNum{ 0 }
	{

	}
	LoginResult(unsigned loginResultNum_)
		: Header(CMD::LOGIN_RESULT, sizeof(LoginResult)), loginResultNum(loginResultNum_)
	{

	}
};

struct LogoutResult : public Header
{
	unsigned logoutResultNum;
	LogoutResult(unsigned logoutResultNum_) :Header(CMD::LOGOUT_RESULT, sizeof(LogoutResult)), logoutResultNum(logoutResultNum_) {}
	LogoutResult() :Header(CMD::LOGOUT_RESULT, sizeof(LogoutResult)), logoutResultNum(0) {}
};