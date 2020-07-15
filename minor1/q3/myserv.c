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
int ssfd[MAX], portno[] = {37775,37776,37777}, cnt = 0;
long cpid[3]={0};

struct sockaddr_in serv_addr, cli_addr;
pthread_t tids[MAX];
struct pollfd spfd[MAX];

void hdfn(int sig, siginfo_t *siginfo, void *context){
	long currpid = (long) siginfo->si_pid;
	for(int i=0;i<3;i++){
		if(currpid == cpid[i]){
			printf("Server S%d reached limit of 3 clients\n",(i+1));
			break;
		}
	}
	//printf("Server pid: %ld, UID: %ld\n",, (long) siginfo->si_uid);
}
int main() {
	struct sigaction act;
	
	act.sa_sigaction = &hdfn;
	act.sa_flags = SA_SIGINFO;
	
	if(sigaction(SIGUSR1, &act, NULL) < 0){
		perror("sigaction");
		return 1;
	}
	
    //initialize server address
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_family = AF_INET;
    
    for(int i=0;i<3;i++){
		serv_addr.sin_port = htons(portno[i]);
		
		if((ssfd[i] = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("socket");
			exit(1);
		}
		int opt = 1;
		if((setsockopt(ssfd[i], SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))) < 0) {
			perror("setsockopt");
			exit(1);
		}
		
		if(bind(ssfd[i], (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		    perror("bind");
		    exit(1);
		}
		if(listen(ssfd[i], 5) < 0) {
		    perror("listen");
		    exit(1);
		}
		spfd[i].fd = ssfd[i];
		spfd[i].events = POLLIN;
    }
    printf("Super Server Started\n");
    while(1) {
    	int jc = 0;
    	for(int i=0;i<3;i++){
    		if(cpid[i]!=0)
    			continue;
    		spfd[jc].fd = ssfd[i];
			spfd[jc].events = POLLIN;
			jc++;
    	}
    	poll(spfd,jc,-1);
    	for(int i=0;i<jc;i++){
    		if(spfd[i].revents & POLLIN){
    			int c = fork();
    			if(c > 0){
    				cpid[i] = c;
    				close(spfd[i].fd);		
    			}else{
    				dup2(spfd[i].fd,0);
    				char idx[3];
    				sprintf(idx,"%d",i+1);
    				printf("Calling special server %s\n",idx);
    				char *args[] = {"./ss",idx,NULL};
    				execvp(args[0],args);
    			}
    		}
    	}
    }
    return 0;
}
