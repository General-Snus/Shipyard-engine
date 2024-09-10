#include <iostream>

#pragma comment (lib, "Ws2_32.lib")

#include "Server/Server.h"

int main()
{

	Server server;
	server.Setup();

	server.Update();
	
	server.Close();
	system("PAUSE");
}