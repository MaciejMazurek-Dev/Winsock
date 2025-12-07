//WIN32_LEAN_AND_MEAN macro prevents the Winsock.h from being included by the Windows.h header.
#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

//Winsock functions are declared in header file <winsock2.h>, but the actual code that implements them lives in Ws2_32.lib library file.
//Line - #pragma comment(lib, "Ws2_32.lib") - automatically link the Winsock library so socket functions work.
#pragma comment(lib, "Ws2_32.lib")



int main()
{
	WORD windowsSocketVersion = MAKEWORD(2, 2); //We want Winsock in version 2.2
	WSADATA wsaData; //WSADATA structure contains information about the Windows Sockets implementation
	int result; 

	printf("Initializing Winsock...");
	result = WSAStartup(windowsSocketVersion, &wsaData); //Initiate use of WS2_32.dll. Returns 0 if OS supports Winsock version 2.2
	if (result != 0)
	{
		result = WSAStartup(windowsSocketVersion, &wsaData);
		printf("WSAStartup failed  with error: %d\n", result);
		return 1;
	}
	
	//Cleanup
	result = WSACleanup();
	if (result != 0)
	{
		printf("Cleanup failed with error: %d\n", result);
		return 1;
	}


	return 0;
}
