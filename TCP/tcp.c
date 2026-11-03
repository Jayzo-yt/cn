/* ==================== SENDER / CLIENT ==================== */

#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>


int main(){
	int sock_fd;
	struct sockaddr_in serveraddr;
	char buffer[1024];

	sock_fd = socket(AF_INET,SOCK_STREAM,0);
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(8080);

	inet_pton(AF_INET,"127.0.0.1",&serveraddr.sin_addr);

	connect(sock_fd,(struct sockaddr *)&serveraddr,sizeof(serveraddr));

	printf("Connection established\n");

	while(1){
		printf("\nFrom Client to Server:");
		fgets(buffer,sizeof(buffer),stdin);
		buffer[strcspn(buffer,"\n")]='\0';
		send(sock_fd,buffer,strlen(buffer)+1,0);
		if(strcmp(buffer,"exit")==0){
			break;
		}

		memset(buffer,0,sizeof(buffer));
		recv(sock_fd,buffer,sizeof(buffer),0);
		printf("Message from Server: %s\n",buffer);
	}

	close(sock_fd);
}
		
			

/* ==================== RECEIVER / SERVER ==================== */

#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>

int main(){
	int server_fd,client_fd;
	struct sockaddr_in serveraddr;
	char buffer[1024];
	
	server_fd = socket(AF_INET,SOCK_STREAM,0);
	
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(8080);
	serveraddr.sin_addr.s_addr = INADDR_ANY;
	
	bind(server_fd,(struct sockaddr *)&serveraddr, sizeof(serveraddr));

	listen(server_fd,5);
	printf("Searching for client connection!!!\n");
	client_fd = accept(server_fd,NULL,NULL);
	
	printf("Client Connection Accepted\n");

	while(1){
		memset(buffer,0,sizeof(buffer));
		
		recv(client_fd,buffer,sizeof(buffer),0);
		
		printf("From Client to Server: %s\n",buffer);
		
		if(strcmp(buffer,"exit")==0){
                        printf("Connection Disconnected by Client.");
			break;
		}		

		printf("\nEnter message to client:");
		fgets(buffer,sizeof(buffer),stdin);

		buffer[strcspn(buffer,"\n")] = '\0';
		
		send(client_fd, buffer, strlen(buffer)+1,0);
	}

	close(server_fd);
	close(client_fd);

	return 0;
}
