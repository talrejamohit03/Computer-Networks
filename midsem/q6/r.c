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
    printf("Reception started\n");	
    char scbuff[]="Source Code\n";
    while(1) {
        struct pollfd fds[2];

        fds[0].fd = sfd1;
        fds[0].events = POLLIN;
		fds[0].revents = 0;
		fds[1].fd = sfd2;
        fds[1].events = POLLIN;
		fds[1].revents = 0;
        
        poll(fds, 2, -1);
        
        if(fds[0].revents & POLLIN) {
            int clilen = 0, csfd1, csfd2;
			if((csfd1 = accept(sfd1, (struct sockaddr *)&cli_addr, &clilen)) < 0) {
				perror("accept");
				return 1;
			}

			printf("new client C1 connected, waiting for C2\n");
			if((csfd2 = accept(sfd2, (struct sockaddr *)&cli_addr, &clilen)) < 0) {
				perror("accept");
				return 1;
			}
			fflush(stdout);
			
			
			send(csfd1,scbuff,strlen(scbuff)+1,0);
			send(csfd2,scbuff,strlen(scbuff)+1,0);
			if(fork()>0){
				close(csfd1);
				close(csfd2);
			}else{
				dup2(csfd1,0);
				dup2(csfd2,1);
				char *arg[]={"./s",NULL};
				execvp(arg[0],arg);
				
			}
			//printf("S1 ended\n");
        }
		else if(fds[1].revents & POLLIN) {
            int clilen = 0, csfd1, csfd2;
			if((csfd2 = accept(sfd2, (struct sockaddr *)&cli_addr, &clilen)) < 0) {
				perror("accept");
				return 1;
			}

			printf("new client C2 connected, waiting for C1\n");
			if((csfd1 = accept(sfd1, (struct sockaddr *)&cli_addr, &clilen)) < 0) {
				perror("accept");
				return 1;
			}
			fflush(stdout);
			
			send(csfd1,scbuff,strlen(scbuff)+1,0);
			send(csfd2,scbuff,strlen(scbuff)+1,0);
			
			if(fork()>0){
				close(csfd1);
				close(csfd2);
			}else{
				dup2(csfd1,0);
				dup2(csfd2,1);
				char *arg[]={"./s",NULL};
				execvp(arg[0],arg);
				
			}
			//printf("S1 ended\n");
            
        }
    }
    return 0;
}
