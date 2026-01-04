#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib") 

#define DEFAULT_SETTINGS 1
#define DEFAULT_IP "127.0.0.1"
#define DEFAULT_PORT "6666"

#define SEND_BUFFER_LENGTH 256
#define RECEIVE_BUFFER_LENGTH 256

int main()
{
	WSADATA wsaData;
	WORD wsaVersion;
	struct addrinfo* addressInfoResult = NULL;
	struct addrinfo* ptr = NULL;
	struct addrinfo hints;
	char hostAddress[20];
	char port[10];
	int iResult;
	char* pcResult;
	char sendBuffer[SEND_BUFFER_LENGTH];
	char receiveBuffer[RECEIVE_BUFFER_LENGTH];

	wsaVersion = MAKEWORD(2, 2);
	iResult = WSAStartup(wsaVersion, &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed with error code: %d\n", iResult);
		return 1;
	}

	//Fill all bytes of the hints object with zeroes
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	
	
	if (DEFAULT_SETTINGS)
	{
		strcpy_s(hostAddress, sizeof(DEFAULT_IP), DEFAULT_IP);
		strcpy_s(port, sizeof(DEFAULT_PORT), DEFAULT_PORT);
	}
	else
	{
		//Get the server's IP address and port from the user
		printf("Please provide host name or ip address: ");
		fgets(hostAddress, sizeof(hostAddress), stdin);
		hostAddress[strcspn(hostAddress, "\n")] = '\0';
		printf("Please provide port number or service name: ");
		fgets(port, sizeof(port), stdin);
		port[strcspn(port, "\n")] = '\0';
	}
	
	
	//Traslate the host name to an address
	iResult = getaddrinfo(hostAddress, port, &hints, &addressInfoResult);
	if (iResult != 0)
	{
		printf("Getting address info failed with error code: %d\n", iResult);
	}

	//Setup the client socket
	SOCKET clientSocket = INVALID_SOCKET;
	for (ptr = addressInfoResult; ptr != NULL; ptr = ptr->ai_next)
	{
		//Create the client socket
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
		if (iResult == 0)
		{
			printf("Connection successful.\n");
			break;
		}
		else if (iResult == SOCKET_ERROR)
		{
			closesocket(clientSocket);
			clientSocket = INVALID_SOCKET;
		}
	}
	if (ptr == NULL)
	{
		printf("Connection failed.\n");
		freeaddrinfo(addressInfoResult);
		WSACleanup();
		return 1;
	}
	

	printf("Type a message to send: ");
	pcResult = fgets(sendBuffer, sizeof(sendBuffer), stdin);
	if (pcResult == NULL)
	{
		printf("Getting user input failed with error: %d\n", ferror(stdin));
	}
	//Send data
	iResult = send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);
	if (iResult == SOCKET_ERROR)
	{
		printf("Send failed with error: %d\n", WSAGetLastError());
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}


	//Cleanup 
	closesocket(clientSocket);
	iResult = WSACleanup();
	if (iResult != 0)
	{
		printf("Cleanup failed with error: %d\n", iResult);
		return 1;
	}
	return 0;
}