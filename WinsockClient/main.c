#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib") 


int main()
{
	WSADATA wsaData;
	WORD wsaVersion = MAKEWORD(2, 2);

	int iResult;
	iResult = WSAStartup(wsaVersion, &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed with error code: %d\n", iResult);
		return 1;
	}

	struct addrinfo* addressInfoResult = NULL;
	struct addrinfo* ptr = NULL;
	struct addrinfo hints;


	//Fills up all bytes of the object hints with zeroes
	memset(&hints, 0, sizeof hints);

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	
	//Get servers ip address and port from the user
	char hostAddress[20];
	char port[10];
	printf("Please provide host name or ip address: ");
	fgets(hostAddress, sizeof(hostAddress), stdin);
	hostAddress[strcspn(hostAddress, "\n")] = '\0';
	printf("Please provide port number or service name: ");
	fgets(port, sizeof(port), stdin);
	port[strcspn(port, "\n")] = '\0';
	
	//Traslate host name to an address
	iResult = getaddrinfo(hostAddress, port, &hints, &addressInfoResult);
	if (iResult != 0)
	{
		printf("Getting address info failed with error code: %d\n", iResult);
	}

	SOCKET clientSocket = INVALID_SOCKET;
	
	for (ptr = addressInfoResult; ptr != NULL; ptr = ptr->ai_next)
	{
		//Create client socket
		clientSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (clientSocket == INVALID_SOCKET)
		{
			printf("Creating socket failed with error: %ld\n", WSAGetLastError());
			continue;
		}
		//Connect to the server
		char stringBuffer[180];
		char* stringPointer = NULL;
		struct sockaddr_in* ipv4Address = (struct sockaddr_in*)ptr->ai_addr;
		stringPointer = InetNtopA(ptr->ai_family, &(ipv4Address->sin_addr) , stringBuffer, sizeof(stringBuffer));
		if (stringPointer == NULL)
		{
			printf("Converting ip address back to presentation format failed with error: %d\n", WSAGetLastError());
		}
		printf("Connecting to: %s\n", stringPointer);
		iResult = connect(clientSocket, ptr->ai_addr, ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(clientSocket);
			clientSocket = INVALID_SOCKET;
		}
	}
	if (ptr == NULL)
	{
		printf("Client failed to connect\n");
		freeaddrinfo(addressInfoResult);
		WSACleanup();
		return 1;
	}

	return 0;
}