#include <iostream>
#include <WS2tcpip.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main()
{
	string ipAddress = "127.0.0.1";
	int port = 54000;

	//Инициализируем winsock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		cerr << "Cant's start wonsock, Err #" << wsResult << endl;
		return;
	}

	//Создаем сокет
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cerr << "Cant's start wonsock, Err #" << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}


	//
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	//Коннектимся к серверу
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		cerr << "Can't connect to server, Err #" << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return;
	}

	//while: не получил и не оптправил данные
	char buf[5000];
	string userInput;

	do
	{
		cout << "> ";
		getline(cin, userInput);

		if (userInput.size() > 0)
		{
			//Отправляем текст
			int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			if (sendResult != SOCKET_ERROR)
			{
				//Ждем
				ZeroMemory(buf, 5000);
				int bytesReceived = recv(sock, buf, 5000, 0);
				if (bytesReceived > 0)
				{
					//Ответ от сервера
					cout << "SERVER>" << string(buf, 0, bytesReceived) << endl;
				}
			}
		}
	} while (userInput.size() > 0);

	//Закрывание

	closesocket(sock);
	WSACleanup();

}