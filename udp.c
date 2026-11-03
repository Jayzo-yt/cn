/* ==================== SENDER / CLIENT ==================== */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>


int main(){
	int sock_fd;
	struct sockaddr_in serveraddr;
	socklen_t slen = sizeof(serveraddr);
	char buffer[1024];

	sock_fd = socket(AF_INET,SOCK_DGRAM,0);

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(8080);
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	printf("Client Establishing connection\n");
	while(1){
		printf("Client:");
		fgets(buffer , sizeof(buffer), stdin);
		buffer[strcspn(buffer,"\n")]='\0';
		
		sendto(sock_fd,buffer,strlen(buffer)+1,0,(struct sockaddr *)&serveraddr,slen);
		
		if(strcmp(buffer,"exit")==0){
			break;
		}
		memset(buffer,0,sizeof(buffer));
		int n = recvfrom(sock_fd,buffer,sizeof(buffer),0,(struct sockaddr *)&serveraddr,&slen);
		buffer[n]='\0';
		
		printf("\nServer : %s\n",buffer);
	}
	close(sock_fd);
}
		

/* ==================== RECEIVER / SERVER ==================== */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>


int main(){
	int sockfd;
	struct sockaddr_in serveraddr,clientaddr;
	char buffer[1024];
	socklen_t clen = sizeof(clientaddr);

	sockfd = socket(AF_INET,SOCK_DGRAM,0);
	
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(8080);
	serveraddr.sin_addr.s_addr = INADDR_ANY;

	bind(sockfd,(struct sockaddr *)&serveraddr, sizeof(serveraddr));

	while(1){
		int n = recvfrom(sockfd,buffer,sizeof(buffer),0,(struct sockaddr *)&clientaddr,&clen);
		buffer[n]='\0';
		printf("\nClient: %s\n",buffer);
		
		if(strcmp(buffer,"exit")==0){
			printf("Done.");
			break;
		}

		memset(buffer,0,sizeof(buffer));
		printf("Server: ");
		fgets(buffer,sizeof(buffer),stdin);
		buffer[strcspn(buffer,"\n")]='\0';

		sendto(sockfd,buffer,strlen(buffer)+1,0,(struct sockaddr *)&clientaddr,clen);

	}

	close(sockfd);
}
