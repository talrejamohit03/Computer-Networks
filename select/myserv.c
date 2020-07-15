#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <poll.h>
#include <signal.h>
#include <fcntl.h>

#define MAX 100
int sfd, nsfds[MAX], portno = 37777, cnt = 0;
struct sockaddr_in serv_addr, cli_addr, cli_addr_list[MAX];

void sendData(char buff[],int curr){
	//printf("%s being written to all\n",buff);
	for(int i = 0; i < cnt; i++) {
        if(i != curr) {
            write(nsfds[i], buff, strlen(buff)+1);
        }
    }
}

int max(int x, int y) {
	return (x>y)?x:y;  
} 
int main() {

    //initialize server address
    //memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);

    //create socket
    if((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    //setsockopt to reuse the same address
    int opt = 1;
    if((setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))) < 0) {
        perror("setsockopt");
        exit(1);
    }

    //bind address to socket fd
    if(bind(sfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    if(listen(sfd, 5) < 0) {
        perror("listen");
        exit(1);
    }
    printf("Server started\n");	
    
    fd_set rset;
    //clearing the descriptor set
	FD_ZERO(&rset);
    
    while(1) {
    	
		//setting sfd in read set
		FD_SET(sfd,&rset);
		
		//get maxfd
		int maxfd = sfd;
		
		for(int i = 0; i < cnt; i++) {
			maxfd = max(maxfd,nsfds[i]);
			FD_SET(nsfds[i],&rset);
		}
		maxfd++;
		select(maxfd,&rset,NULL,NULL,NULL);//timeout is NULL it will wait forever
        
        
        if(FD_ISSET(sfd,&rset)) {
        	//printf("sfd is set\n");
            int clilen = 0;
			if((nsfds[cnt] = accept(sfd, (struct sockaddr *)&cli_addr, &clilen)) < 0) {
				perror("accept");
				return 1;
			}

			printf("new client added\n");

			bcopy((char *)&cli_addr,(char *)&cli_addr_list[cnt],sizeof(cli_addr));
			
			cnt++;
            continue;
        }

        for(int i = 0; i < cnt; i++) {
            if(FD_ISSET(nsfds[i],&rset)) {
            	//printf("some nsfd is set\n");
            	char buff[MAX];
                read(nsfds[i], &buff, MAX);
                sendData(buff,i);
            }
        }
    }
    return 0;
}
