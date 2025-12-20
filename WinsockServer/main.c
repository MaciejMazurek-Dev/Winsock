//WIN32_LEAN_AND_MEAN macro prevents the old version 1 of Winsock (Winsock.h) from being included by the Windows.h header which conflicts with new version in header file winsock2.h
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

//Winsock functions are declared in header file <winsock2.h>, but the actual code that implements them lives in Ws2_32.lib library file.
//Pragma line below automatically link the Winsock library so socket functions work.
#pragma comment(lib, "Ws2_32.lib")

#define IPV4_ADDRESS "127.0.0.1"
#define PORT 6666
#define BUFFER_LENGTH 256

int main()
{
	//INITIALIZE
	WORD windowsSocketVersion = MAKEWORD(2, 2);
	//WSADATA contains information about the Windows Sockets implementation. Btw. WSA means WinSock API
	WSADATA wsaData;
	printf("Initializing Winsock\n");
	//WSAStartup initiate use of WS2_32.dll (Winsock DLL) and returns 0 if everythings is OK.
	int result = WSAStartup(windowsSocketVersion, &wsaData); 
	if (result != 0)
	{
		printf("WSAStartup failed with error: %d\n", result);
		return 1;
	}
	

	//SERVER SETUP
	SOCKET listenSocket = INVALID_SOCKET;
	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET)
	{
		printf("Socket construction failed with error: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	//Bind Socket to the address
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	//Convert IP text address into binary network format
	inet_pton(AF_INET, (PCWSTR)(IPV4_ADDRESS), &serverAddress.sin_addr.S_un.S_addr);
	serverAddress.sin_port = htons(PORT);
	result = bind(listenSocket, &serverAddress, sizeof(serverAddress));
	if (result == SOCKET_ERROR)
	{
		printf("Binding a socket failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//Listening on a Socket
	result = listen(listenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR)
	{
		printf("Listen failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//Accepting a connection
	printf("Listening for incoming connections...\n");
	SOCKET clientSocket;
	clientSocket = INVALID_SOCKET;
	clientSocket = accept(listenSocket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET)
	{
		printf("Accepting client connection failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//Receiving data
	char receivingBuffer[BUFFER_LENGTH];
	int bufferLength = BUFFER_LENGTH;
	int receiveResult;
	do
	{
		receiveResult = recv(clientSocket, &receivingBuffer, bufferLength, 0);
		if (receiveResult > 0)
		{
			printf("Bytes received: %d\n", receiveResult);
		}
		else if(receiveResult == 0)
		{
			printf(receivingBuffer);
			printf("All data received.\n");
		}
		else
		{
			printf("Receiving data failed with error: %d\n", WSAGetLastError());
		}
	} while (receiveResult > 0);



	//CLEANUP
	printf("Cleaning up...\n");
	closesocket(listenSocket);
	closesocket(clientSocket);
	result = WSACleanup();
	if (result != 0)
	{
		printf("Cleanup failed with error: %d\n", result);
		return 1;
	}

	return 0;
}
