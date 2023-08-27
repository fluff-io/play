// Server side C/C++ program to demonstrate Socket
// programming
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#define PORT 8080
int server_fd, sockets[100], socket_len=0;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);
int true = 1;
pthread_t read_thread, socket_thread;

int readData(char* buf, int num){
	return read(sockets[num], buf, sizeof(buf));
}
void sendData(const char* data, int num){
	send(sockets[num], data, strlen(data), 0);
}
void* readLoop(void* vargp){
	char buf[1024];
	while(true){
		strcpy(buf, "");
		readData(buf, *(int*)vargp);
		if(strcmp(buf, "") == 0){
			printf("Socket %d has closed.\n", *(int*)vargp);
			return NULL;
		}
		printf("Data from socket %d: %s\n", *(int*)vargp, buf);
	}
}
void wait_for_socket(){
	if ((sockets[socket_len]
		= accept(server_fd, (struct sockaddr*)&address,
				(socklen_t*)&addrlen))
		< 0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}else{
		socket_len += 1;
	}
}
void* socketLoop(void* vargp){
	while(true){
		printf("Waiting for a socket...\n");
		wait_for_socket();
		printf("New socket: %d\n", socket_len - 1);
		sendData("Hi!", socket_len - 1);
		void* val = malloc(sizeof(int));
		*(int*)val = socket_len - 1;
		printf("%p\n", val);
    	pthread_create(&read_thread, NULL, readLoop, val);
	}
}
int main()
{
  sleep(5);
	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
	if (setsockopt(server_fd, SOL_SOCKET,
				SO_REUSEADDR | SO_REUSEPORT, &opt,
				sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// Forcefully attaching socket to the port 8080
	if (bind(server_fd, (struct sockaddr*)&address,
			sizeof(address))
		< 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	printf("Waiting for sockets...\n");
	socketLoop(NULL);
	return 0;
}
