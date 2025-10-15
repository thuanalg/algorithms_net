#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") // cần nếu biên dịch bằng MSVC

#if 0
#define SERVER_PORT 12345
#define BUFFER_SIZE 1500

int
main()
{
	WSADATA wsaData;
	SOCKET sockfd;
	struct sockaddr_in serverAddr, clientAddr;
	char buffer[BUFFER_SIZE];
	int recvLen;
	int clientAddrLen = sizeof(clientAddr);
	long long int total = 0;

	// 1. Khởi tạo Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("WSAStartup failed.\n");
		return 1;
	}

	// 2. Tạo socket UDP
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == INVALID_SOCKET) {
		printf("Socket creation failed: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// 3. Bind socket vào cổng
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(SERVER_PORT);

	if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) ==
	    SOCKET_ERROR) {
		printf("Bind failed: %d\n", WSAGetLastError());
		closesocket(sockfd);
		WSACleanup();
		return 1;
	}

	printf("UDP server is listening on port %d...\n", SERVER_PORT);

	// 4. Nhận dữ liệu từ client
	while (1) {
		recvLen = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
		    (struct sockaddr *)&clientAddr, &clientAddrLen);
		if (recvLen == SOCKET_ERROR) {
			//printf("recvfrom failed: %d\n", WSAGetLastError());
			continue;
		}
		total += recvLen;
		buffer[recvLen] = '\0'; // Null terminate
#if 0
		printf("Received from %s:%d -> %s\n",
		    inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port),
		    buffer);
#else
		printf("len: %d, total: %lld\n", recvLen, total);
#endif
	}

	// 5. Cleanup
	closesocket(sockfd);
	WSACleanup();
	return 0;
}
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") // chỉ cần nếu biên dịch MSVC

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
#define BUFFER_SIZE 1316 // phù hợp với ffmpeg/ffplay MPEG-TS

int
main()
{
	WSADATA wsaData;
	SOCKET sockfd;
	struct sockaddr_in serverAddr;
	char buffer[BUFFER_SIZE];
	int i;

	// 1. Khởi tạo Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("WSAStartup failed.\n");
		return 1;
	}

	// 2. Tạo socket UDP
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == INVALID_SOCKET) {
		printf("Socket creation failed: %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// 3. Thiết lập địa chỉ server
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

	printf("Sending UDP packets to %s:%d...\n", SERVER_IP, SERVER_PORT);

	// 4. Gửi dữ liệu liên tục (demo)
	for (i = 0; i < 1000; i++) {
		// fill buffer với dữ liệu mẫu
		memset(buffer, i % 256, BUFFER_SIZE);

		int sent = sendto(sockfd, buffer, BUFFER_SIZE, 0,
		    (struct sockaddr *)&serverAddr, sizeof(serverAddr));
		if (sent == SOCKET_ERROR) {
			printf("sendto failed: %d\n", WSAGetLastError());
			break;
		}

		printf("Packet %d sent (%d bytes)\n", i + 1, sent);
		Sleep(10); // delay 10ms để tránh gửi quá nhanh
	}

	// 5. Cleanup
	closesocket(sockfd);
	WSACleanup();
	return 0;
}

#endif
