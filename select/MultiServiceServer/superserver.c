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
int sfd1,sfd2, nsfds[MAX], portno1 = 37777, portno2 = 37778,cnt = 0;
struct sockaddr_in serv_addr, cli_addr;

int max(int x, int y) {
	return (x>y)?x:y;  
} 
int main() {

    //initialize server address
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno1);

    //create socket
    if((sfd1 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
	if((sfd2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }
    //setsockopt to reuse the same address
    int opt = 1;
    if((setsockopt(sfd1, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))) < 0) {
        perror("setsockopt");
        exit(1);
    }
	if((setsockopt(sfd2, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))) < 0) {
        perror("setsockopt");
        exit(1);
    }
    //bind address to socket fd
    if(bind(sfd1, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        exit(1);
    }
	serv_addr.sin_port = htons(portno2);
	if(bind(sfd2, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        exit(1);
    }
    if(listen(sfd1, 5) < 0) {
        perror("listen");
        exit(1);
    }
    if(listen(sfd2, 5) < 0) {
        perror("listen");
        exit(1);
    }
    printf("Server started\n");	
    
    fd_set rset;
    //clearing the descriptor set
	FD_ZERO(&rset);
    while(1) {
    	//setting sfd in read set
		FD_SET(sfd1,&rset);
		FD_SET(sfd2,&rset);
		//get maxfd
		int maxfd = max(sfd1, sfd2) + 1;
		
		select(maxfd,&rset,NULL,NULL,NULL);//timeout is NULL it will wait forever
		
        if(FD_ISSET(sfd1,&rset)) {
            int clilen = 0, csfd;
			if((csfd = accept(sfd1, (struct sockaddr *)&cli_addr, &clilen)) < 0) {
				perror("accept");
				return 1;
			}

			printf("new client connected to S1\n");
			fflush(stdout);
			if(fork()==0){
				dup2(csfd,1);
				dup2(csfd,0);
				char *arg[]={"./s1",NULL};
				execvp(arg[0],arg);
				close(csfd);
			}
			//printf("S1 ended\n");
        }
		if(FD_ISSET(sfd2,&rset) ){
            int clilen = 0, csfd;
			if((csfd = accept(sfd2, (struct sockaddr *)&cli_addr, &clilen)) < 0) {
				perror("accept");
				return 1;
			}

			printf("new client connected to S2\n");
		
			if(fork()==0){
				dup2(csfd,1);
				dup2(csfd,0);
				char *arg[]={"./s2",NULL};
				execvp(arg[0],arg);
				close(csfd);
			}
			//printf("S1 ended\n");
            
        }
    }
    return 0;
}
