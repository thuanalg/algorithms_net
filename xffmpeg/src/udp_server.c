#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib") // cần nếu biên dịch bằng MSVC

#if 0
#define SERVER_PORT 12345
#define BUFFER_SIZE 15000

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
	FILE *fp = 0;
	int delta = 0;
	time_t t0 = 0;
	time_t t1 = 0;
	t0 = time(0);
	t1 = time(0);
	fp = fopen("C:/Users/DEll/Desktop/z.mp4", "w+");
	if (!fp) {
		exit(1);
	}
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
	u_long mode = 1; // 1 = non-blocking
	ioctlsocket(sockfd, FIONBIO, &mode);
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
		t1 = time(0);
		recvLen = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
		    (struct sockaddr *)&clientAddr, &clientAddrLen);
		if (recvLen == SOCKET_ERROR) {
			Sleep(1000);
			delta = t1 - t0;
			if (delta > 20) {
				break;
			}
			continue;
		}
		total += recvLen;
		fwrite(buffer, 1, recvLen, fp);
		buffer[recvLen] = '\0'; // Null terminate
#if 0
		printf("Received from %s:%d -> %s\n",
		    inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port),
		    buffer);
#else
		printf("len: %d, total: %lld\n", recvLen, total);
#endif
		t0 = t1;
	}

	// 5. Cleanup
	closesocket(sockfd);
	WSACleanup();
	fclose(fp);
	return 0;
}

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
#else
// tcp_server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif

#define PORT 12345
#define BUF_SIZE 10240

int
main()
{
	FILE *fp = 0;
	int delta = 0;
	time_t t0 = 0;
	time_t t1 = 0;
	t0 = time(0);
	t1 = time(0);
	fp = fopen("C:/Users/DEll/Desktop/z.mp4", "w+");
	if (!fp) {
		exit(1);
	}
#ifdef _WIN32
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("WSAStartup failed\n");
		return 1;
	}
#endif

	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0) {
		perror("socket");
		return 1;
	}

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY; // nghe tất cả interface
	addr.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		return 1;
	}

	if (listen(server_fd, 5) < 0) {
		perror("listen");
		return 1;
	}

	printf("TCP server listening on port %d...\n", PORT);

	while (1) {
		struct sockaddr_in client_addr;
		socklen_t client_len = sizeof(client_addr);
		int client_fd = accept(
		    server_fd, (struct sockaddr *)&client_addr, &client_len);
		if (client_fd < 0) {
			perror("accept");
			continue;
		}

		char client_ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &client_addr.sin_addr, client_ip,
		    sizeof(client_ip));
		printf("Client connected: %s:%d\n", client_ip,
		    ntohs(client_addr.sin_port));

		char buf[BUF_SIZE];
		int n = 0;
		u_long mode = 1; // 1 = non-blocking
		ioctlsocket(client_fd, FIONBIO, &mode);
		while (1) {
			t1 = time(0);
			n = recv(client_fd, buf, sizeof(buf), 0);
			if (n < 1) {
				Sleep(1000);

				delta = t1 - t0;
				if (delta > 10) {
					break;
				}
				continue;
			}

			fwrite(buf, 1, n, fp);
			t0 = t1;
		}
		closesocket(client_fd);

		printf("Client disconnected\n");
#ifdef _WIN32
		
#else
		close(client_fd);
#endif
		if (delta > 10) {
			break;
		}
	}

#ifdef _WIN32
	closesocket(server_fd);
	WSACleanup();
#else
	close(server_fd);
#endif
	fclose(fp);
	return 0;
}

#endif
