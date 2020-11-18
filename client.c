#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"

int main(int argc, char *argv[]){
	int sockfd; //socket number
	int len_send, len_recv; //length of sended message, received message
	char msg_send[1024], msg_recv[1024]; //sended message, received message
	struct sockaddr_in servaddr;

	if(argc!=3){
		printf("input error");
		exit(0);
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0))<0){
		printf("socket error");
		exit(0);
	}

	memset(&servaddr, 0x00, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr)<=0){
		printf("inet_pton error");
		exit(0);
	}

	if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))<0){
		printf("connect error");
		exit(0);
	}
	else{
		printf("connected");
	}

	while(1){
		fgets(msg_send, 1024, stdin);
		len_send = write(sockfd, msg_send, strlen(msg_send));
		len_recv = read(sockfd, msg_recv, 1023);
		msg_recv[len_recv] = 0;

		printf("%s", msg_recv);
	}
	close(sockfd);
	return 0;
}
