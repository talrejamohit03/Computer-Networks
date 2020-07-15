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
int mysfd, ssfd[MAX], nsfds[MAX], myportno = 37774, portno[] = {37775,37776,37777,37778,37779}, cnt = 0;
struct sockaddr_in serv_addr, cli_addr;

int main() {

    //initialize server address
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(myportno);
    
    if((mysfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	    perror("socket");
	    exit(1);
	}
	int opt = 1;
	if((setsockopt(mysfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))) < 0) {
    	perror("setsockopt");
    	exit(1);
	}
	
	if(bind(mysfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        exit(1);
    }
    if(listen(mysfd, 5) < 0) {
        perror("listen");
        exit(1);
    }
    printf("Proxy Server Started\n");
    while(1) {
    	int clilen = 0, csfd;
		if((nsfds[cnt] = accept(mysfd, (struct sockaddr *)&cli_addr, &clilen)) < 0) {
			perror("accept");
			return 1;
		}
		printf("new client added\n");
		char ask[]="Enter service number: ";
		send(nsfds[cnt],ask,strlen(ask)+1,0);
		int sno;
		recv(nsfds[cnt],&sno,sizeof(sno),0);
		printf("Received request for special server: %d\n",sno);
		
		if((ssfd[cnt] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		    perror("socket");
		    exit(1);
		}
		serv_addr.sin_port = htons(portno[sno-1]);
		if(connect(ssfd[cnt], (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        	perror("Error connecting, Special server not running");
        	return 1;
    	}
    	if(dup2(nsfds[cnt],ssfd[cnt])){
    		perror("dup2");
    	}
        cnt++;
    }
    return 0;
}
