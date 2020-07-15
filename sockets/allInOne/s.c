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

#include<sys/wait.h>

#include<string.h>
#include<sys/stat.h>
#include<stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#define MAX 100
long pids[10],epids[10];
int active[10]={0};
int sfd, nsfds[MAX], portno = 37777, cnt = 0;
struct sockaddr_in serv_addr, cli_addr, cli_addr_list[MAX];

void sendData(char buff[],int curr){
	//printf("%s being written to all\n",buff);
	for(int i = 0; i < cnt; i++) {
        if(active[i] == 1 && i != curr) {
            write(nsfds[i], buff, strlen(buff)+1);
        }
    }
}

int max(int x, int y) {
	return (x>y)?x:y;  
} 

void hdfn1(int sig, siginfo_t *siginfo, void *context){
	printf("Echo server with pid: %ld terminated\n",(long) siginfo->si_pid);
	for(int i=0;i<cnt;i++){
		if(epids[i] == (long) siginfo->si_pid){
			printf("Client %d joined back to main server\n", i);
			active[i] = 1;
			break;
		}
	}
}

int search(int pid){
	for(int i=0;i<cnt;i++){
		if(pids[i]==pid)
			return i;
	}
	return -1;
}

void hdfn(int signo){
	if(signo == SIGUSR1){
		printf("Received signal from p4\n");
		fflush(stdout);
		if(cnt<=0){
			printf("No clients available\n");
			fflush(stdout);
			return;
		}
		int i=0;
		for(i;i<cnt;i++){
			if(active[i]==1){
				break;
			}
		}
		if(i == cnt){
			printf("All clients already attached to echo\n");
			fflush(stdout);
			return;
		}
		printf("Client %d being transferred to echo server\n",i);
		fflush(stdout);
		active[i] = 0;
		long ech;
		if((ech = fork()) > 0){
			epids[i] = ech;
		}
		else{
			dup2(nsfds[i],0);
			dup2(nsfds[i],1);
			char *argv[]={"./echo",NULL};
			execvp(argv[0],argv);
		}
		printf("Echo Success\n");
	}
}
int main() {    
    struct sigaction act;
	
	act.sa_sigaction = &hdfn1;
	act.sa_flags = SA_SIGINFO;
	
	if(sigaction(SIGUSR2, &act, NULL) < 0){
		perror("sigaction");
		return 1;
	}
	
	if(mkfifo("p2fifo",0666) < 0){
		//perror("mkfifo");
		//return 1;
	}
	//for process p5
	printf("popen for p5\n");
	FILE *fptr = popen("./p5","w");
	if(fptr == NULL){
		perror("popen");
		return 1;
	}
	int p5fd = fileno(fptr);
	if(p5fd < 0){
		perror("fileno");
		return 1;
	}
	dup2(p5fd,1);
	printf("p5 executed\n");
	//for process p1
	int pp[2];
	pipe(pp);
	if(fork()>0){
		close(pp[1]);
	}else{
		close(pp[0]);
		dup2(pp[1],1);
		char *argv[]={"./p1",NULL};
		execvp(argv[0],argv);
	}
	printf("p1 started\n");
	
	//for process p2
	if(fork()==0){
		char *argv[]={"./p2",NULL};
		execvp(argv[0],argv);
	}
	int ffd = open("p2fifo",O_RDONLY|O_NONBLOCK);
	printf("p2 started\n");
	
	//for process p3
	int fd = fileno(popen("./p3","r"));
	printf("p3 started\n");
	
	//for process p4
	signal(SIGUSR1,hdfn);
	printf("p4 signal handler created\n");
	
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
    while(1) {
    	fd_set rset;
    	//clearing the descriptor set
		FD_ZERO(&rset);
		//setting all fds in read set
		FD_SET(sfd,&rset);
		int maxfd = sfd;
		FD_SET(STDIN_FILENO,&rset);
		maxfd = max(maxfd,STDIN_FILENO);
		
		FD_SET(pp[0],&rset);
		maxfd = max(maxfd,pp[0]);
		FD_SET(ffd,&rset);
		maxfd = max(maxfd,ffd);
		FD_SET(fd,&rset);
		maxfd = max(maxfd,fd);
		
		for(int i = 0; i < cnt; i++) {
			if(active[i] == 1){
				maxfd = max(maxfd,nsfds[i]);
				FD_SET(nsfds[i],&rset);
			}
		}
		maxfd++;
		//timeout is NULL it will wait forever
		if(select(maxfd,&rset,NULL,NULL,NULL) < 0)
			continue;
        //printf("in loop\n");
        if(FD_ISSET(STDIN_FILENO,&rset)){
			char buff[MAX];
			scanf("%[^\n]%*c",buff);
			sendData(buff,-1);
		}
		if(FD_ISSET(pp[0],&rset)){
			char buff[MAX];
			read(pp[0],&buff,MAX);
			sendData(buff,-1);
		}
		if(FD_ISSET(ffd,&rset)){
			char buff[MAX];
			read(ffd,&buff,MAX);
			sendData(buff,-1);
		}
		if(FD_ISSET(fd,&rset)){
			char buff[MAX];
			read(fd,&buff,MAX);
			sendData(buff,-1);
		}
        if(FD_ISSET(sfd,&rset)) {
        	printf("sfd is set\n");
            int clilen = 0;
			if((nsfds[cnt] = accept(sfd, (struct sockaddr *)&cli_addr, &clilen)) < 0) {
				perror("accept");
				return 1;
			}

			printf("new client added\n");
			fflush(stdout);
			bcopy((char *)&cli_addr,(char *)&cli_addr_list[cnt],sizeof(cli_addr));
			active[cnt] = 1;
			cnt++;
            continue;
        }

        for(int i = 0; i < cnt; i++) {
            if(FD_ISSET(nsfds[i],&rset)) {
            	printf("some nsfd is set\n");
            	char buff[MAX];
                read(nsfds[i], &buff, MAX);
                sendData(buff,i);
            }
        }
    }
    return 0;
}
