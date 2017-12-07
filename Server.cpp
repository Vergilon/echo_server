#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <sstream>

#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main()
{
	//������������� winsock

	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsok = WSAStartup(ver, &wsData);

	if (wsok != 0)
	{
		cerr << "Can't Initialize winsock! Quitting" << endl;
		return;
	}


	//��������� ������

	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't Initialize socket! Quitting" << endl;
		return;
	}


	//������ ����� ����� ip � ����

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;
	//����� ����� ������������ inet_pton (�������������� IPv4 � IPv6)

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// ������� winsock ����� ��� ��������

	listen(listening, SOMAXCONN);

	fd_set master;
	FD_ZERO(&master);

	FD_SET(listening, &master);

	while(true)
	{
		fd_set copy = master;

		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];
			if (sock == listening)
			{
				SOCKET client = accept(listening, nullptr, nullptr);

				FD_SET(client, &master);

				string welcomeMsg = "Welcome to the Chat.\r\n";
				send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
			}
			else
			{
				char buf[5000];
				ZeroMemory(buf, 5000);

				int bytesIn = recv(sock, buf, 5000, 0);
				if (bytesIn <= 0)
				{
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{

					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && outSock != sock)
						{

							ostringstream ss;
							ss << "Socket #" << sock << ": " << buf << "\r\n";
							string strOut = ss.str();
							send(outSock, strOut.c_str(), strOut.size() + 1, 0);
						}
					}
				}
			}
		}
	}
	//���� ����������

	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

	char host[NI_MAXHOST];
	char service[NI_MAXHOST];

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST);
	// � ����� memset(host, 0, NI_MAXHOST);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << "Connected on port" << service << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << "connected on port" <<
			ntohs(client.sin_port) << endl;
	}

	//��������� �������

	closesocket(listening);

	//While: ��������� � ���������� ��������� ������� �������

	char buf[5000];

	while (true)
	{
		ZeroMemory(buf, 5000);

		//���� ����������� ���������� �� �������

		int bytesReceived = recv(clientSocket, buf, 5000, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			cerr << "Error in recv(). Quitting" << endl;
			break;
		}
		if (bytesReceived == 0)
		{
			cout << "client disconnected " << endl;
			break;
		}

		cout << string(buf, 0, bytesReceived) << endl;
		//��������� ��������� ������� �������

		send(clientSocket, buf, bytesReceived + 1, 0);


	}
	//�������� ������

	closesocket(clientSocket);

	//�������� winsock
	WSACleanup();

}