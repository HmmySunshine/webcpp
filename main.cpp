#include "server.h"
#pragma comment(lib, "ws2_32.lib")

int main()
{
	Server server;
	if (!server.Init("127.0.0.1", 2345))
	{
		"Failed to initialize server";
		return 1;
	}
	server.Start();
	system("pause");
	return 0;
}