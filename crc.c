/* ==================== SENDER ==================== */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX 256

struct M{
	int dl,gl,cl;
	char code[MAX],gen[MAX];
};


int main(){
	char data[MAX],gen[MAX],tmp[MAX],code[MAX];

	int sockfd = socket(AF_INET,SOCK_DGRAM,0);

	struct sockaddr_in server;

	server.sin_family = AF_INET;
	server.sin_port = htons(8080);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");

	socklen_t slen = sizeof(server);

	
	printf("Enter data bits: ");
	scanf("%255s",data);

	printf("Enter genrator bits: ");
        scanf("%255s",gen);

	int dl = strlen(data);
	int gl = strlen(gen);

	strcpy(tmp,data);

	for(int i=0;i<gl-1;i++){
		tmp[dl+i] = '0';
	}
	tmp[dl+gl-1] = '\0';


	for(int i=0;i<dl;i++){
		if(tmp[i] == '1'){
			for(int j=0;j<gl;j++){
				tmp[i+j] = tmp[i+j] == gen[j] ? '0' : '1';
			}
		}
	}

	strcpy(code,data);
	strcat(code,tmp+dl);

	printf("Codeword: %s\n",code);
	printf("Remainder: %s\n",tmp+dl);
	struct M msg;

	msg.dl = dl;
	msg.gl = gl;
	msg.cl = strlen(code);
	strcpy(msg.code, code);
	strcpy(msg.gen,gen);

	int pos;
	printf("Enter pos to change the code(-1 means dont change): ");
	scanf("%d",&pos);

	if(pos>0){
		msg.code[pos] = msg.code[pos] == '0' ? '1' : '0';
		printf("Changed Codeword: %s\n",msg.code);
	}

	sendto(sockfd,&msg,sizeof(msg),0,(struct sockaddr *)&server,slen);
	close(sockfd);
}

/* ==================== RECEIVER ==================== */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX 256

struct M{
        int dl,gl,cl;
        char code[MAX],gen[MAX];
};

int main(){
	char x[MAX];

	int sockfd = socket(AF_INET,SOCK_DGRAM,0);

        struct sockaddr_in server,client;

        server.sin_family = AF_INET;
        server.sin_port = htons(8080);
        server.sin_addr.s_addr = INADDR_ANY;

        socklen_t slen = sizeof(client);

	bind(sockfd,(struct sockaddr *)&server,sizeof(server));

	struct M msg;

	recvfrom(sockfd,&msg,sizeof(msg),0,(struct sockaddr *)&client,&slen);

	strcpy(x,msg.code);

	for(int i=0;i<msg.dl;i++){
		if(x[i]=='1'){
			for(int j=0;j<msg.gl;j++){
				x[i+j]=x[i+j]==msg.gen[j] ? '0' : '1';
			}
		}	
	}

	
	int zero=0;

	printf("Received codeword: %s\n", msg.code);
        printf("Remainder: ");

	for(int i=msg.dl;i<msg.cl;i++){
		printf("%c",x[i]);
		if(x[i]=='1'){
			zero=1;
		}
	}
	printf("\n");
	if(zero==0){
		printf("No Error Detected");
	}
	else{
	printf("Error Detected");
	}
	close(sockfd);
}
