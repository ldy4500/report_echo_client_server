#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "arpa/inet.h"
#include "netinet/in.h"
#include "pthread.h"

int connect_fd[10];
int fd_end = 0;

void *t_function(void *connect_fd){
	int msg_size;
	int fd;
	fd = *((int *)connect_fd);
	char message[1024];
	while((msg_size = read(fd, message, 1024))!=0){
		write(fd, message, msg_size);
		//broadcast(message);
		printf("%s\n",message);
	}
	close(fd);
}

void *t_functionBroadcast(void *connect_fd){
	int msg_size;
	int fd;
	fd  = *((int *)connect_fd);
	char message[1024];
	while((msg_size = read(fd, message, 1024))!=0){
		broadcast(message);
		printf("%s\n",message);
	}
	close(fd);
}

void *t_functionNonEcho(void *connect_fd){
	int msg_size;
	int fd;
	fd = *((int *)connect_fd);
	char message[1024];
	while((msg_size = read(fd, message, 1024))!=0){
		printf("%s\n",message);
	}
	close(fd);
}

void broadcast(char *message){
	for(int i=0;i<fd_end;i++){
		write(connect_fd[i], message, strlen(message));
	}
}


int main(int argc, char *argv[]){
	int server_fd;// listen, connect socket
	char message[1024];
	struct sockaddr_in serv_addr, client_addr; //server & client address
	unsigned int len;
	int msg_size;
	char client_ip[20];
	pthread_t p_thread[10];
	int end = 0;
	int thread_id;
	int status;
	int echo = 0;


	len = sizeof(client_addr);

	if(argc < 2){
		printf("port miss");
		exit(0);
	}

	if(argc == 2){
		echo = 0;
	}
	
	if(argc == 3){
		if(strcmp(argv[2],"-e")==0){
			echo = 1;
		}
		else{
			printf("not correct option");
			exit(0);
		}
	}

	if(argc == 4){
		if(strcmp(argv[2], "-e") == 0 && strcmp(argv[3], "-b") == 0){
			echo = 2;
		}
		else{
			printf("not correct option");
			exit(0);
		}
	}

	if((server_fd = socket(AF_INET, SOCK_STREAM, 0))<0){
		printf("socket error");
		exit(0);
	}

	memset(&serv_addr, 0x00, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));


	if(bind(server_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0){
		printf("bind error");
		exit(0);
	}

	if(listen(server_fd, 5)<0){
		printf("Can't listen");
		exit(0);
	}

	memset(message, 0x00, sizeof(message));
	printf("wait for client\n");

	while(1){
		connect_fd[fd_end] = accept(server_fd,(struct sockaddr *)&client_addr, &len);
		if(connect_fd[fd_end] < 0){
			printf("accept error");
			exit(0);
		}
	
		printf("connect");
		inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip, sizeof(client_ip));
		printf("%s connected\n",client_ip);

		/*		
		while((msg_size = read(connect_fd, message, 1024))!=0){
			write(connect_fd, message, msg_size);
			printf("%s \n", message);
		}*/
		if(echo == 0){	
			thread_id = pthread_create(&p_thread[end],NULL, t_functionNonEcho,(void*)&connect_fd[fd_end]);
		}
		else if(echo == 1){
			thread_id = pthread_create(&p_thread[end],NULL, t_function,(void*)&connect_fd[fd_end]);
		}
		else if(echo == 2){
			thread_id = pthread_create(&p_thread[end],NULL, t_functionBroadcast,(void*)&connect_fd[fd_end]);
		}

		end++;
		fd_end++;
		if(thread_id < 0){
			printf("tread error");
			exit(0);
		}

		
	}
	for(int i = 0; i<end; i++){
		pthread_join(p_thread[0], (void **)&status);
	}
	close(server_fd);
	return 0;
}


