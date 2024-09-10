#include <string>
#include <thread>

#include "Client/Client.h"


int main()
{
	Client client;
	client.Setup();

	client.Update();

	client.Close();

	system("PAUSE");
}
