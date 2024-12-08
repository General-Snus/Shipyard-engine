#pragma comment (lib, "Ws2_32.lib")

#include "Server/Server.h"

int main()
{
	Server server;
	server.setup();

	server.update();

	server.close();
}
