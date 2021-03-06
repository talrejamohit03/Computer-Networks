#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include<pthread.h>
#include <arpa/inet.h>
#include <string.h>
#include <poll.h>
#include <signal.h>

#define CONTROLLEN CMSG_LEN(sizeof(int))

#define MAX 100
int sfd1,sfd2, nsfds[MAX], portno1 = 37777, portno2 = 37778,cnt = 0;
struct sockaddr_in serv_addr, cli_addr;

static struct cmsghdr *cmptr = NULL;

char *path = "./usoc";
char *path2 = "./usoc2";

pthread_t tid;
int nums[100]={0};//numbers generated by judge
int usfd,nufd;
void *tRunner(void *arg){
	while(1){
		int num;
		recv(nufd,&num,sizeof(num),0);
		nums[num] = 1;
		printf("Judge picked %d\n",num);
	}
}
int send_fd(int usfd, int fd_to_send) {
    
    struct iovec iov[1];
    struct msghdr msg;
    
    char buf[2] = {0, 0};
    /* buf[0] = 0 means null byte flag to recv_fd() 
    	buf[1] = 0 means zero status implies OK */
    iov[0].iov_base = buf;
    iov[0].iov_len = 2;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    
    cmptr = malloc(CONTROLLEN);
    cmptr->cmsg_level = SOL_SOCKET;
    cmptr->cmsg_type = SCM_RIGHTS;
    cmptr->cmsg_len = CONTROLLEN;
    msg.msg_control = cmptr;
    msg.msg_controllen = CONTROLLEN;
    *(int *)CMSG_DATA(cmptr) = fd_to_send;

    if(sendmsg(usfd, &msg, 0) < 0) {
        perror("sendmsg");
        return -1;
    }

    return 0;
}
int main(int argc, char *argv[]) {

	struct sockaddr_un addr;


	if ( (usfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket error");
		exit(1);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, path);
	unlink(path);
	
	if (bind(usfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		perror("bind error");
		exit(1);
	}

	if (listen(usfd, 5) == -1) {
		perror("listen error");
		exit(1);
	}
	struct sockaddr cli;
	int len;
	if((nufd = accept(usfd, (struct sockaddr*)&cli, &len)) < 0) {
	    perror("accept");
	    exit(1);
	}
	printf("Judge connected\n");
	
	int usfd2,nufd2;

	if ( (usfd2 = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket error");
		exit(1);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, path2);
	unlink(path2);
	
	if (bind(usfd2, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		perror("bind error");
		exit(1);
	}

	if (listen(usfd2, 5) == -1) {
		perror("listen error");
		exit(1);
	}
	//struct sockaddr cli_addr;
	//int len;
	if((nufd2 = accept(usfd2, (struct sockaddr*)&cli, &len)) < 0) {
	    perror("accept");
	    exit(1);
	}
	printf("Prize connected\n");
	/*
	int fd1 = open("tmp.txt", O_RDONLY);

    if(send_fd(nufd, fd1) < 0) {
        perror("send_fd");
        exit(1);
    }
	*/
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
    pthread_create(&tid,NULL,tRunner,NULL);
    printf("Distributor started\n");	
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
            int clilen = 0, csfd;
			if((csfd = accept(sfd1, (struct sockaddr *)&cli_addr, &clilen)) < 0) {
				perror("accept");
				return 1;
			}

			printf("new client connected for judge\n");
			fflush(stdout);
			int coup[5];
			for(int i=0;i<5;i++){
				coup[i] = rand()%100;
			}
			send(csfd,coup,sizeof(coup),0);
			if(send_fd(nufd, csfd) < 0) {
        		perror("send_fd");
        		exit(1);
    		}
        }
		if(fds[1].revents & POLLIN) {
            int clilen = 0, csfd;
			if((csfd = accept(sfd2, (struct sockaddr *)&cli_addr, &clilen)) < 0) {
				perror("accept");
				return 1;
			}
			printf("new client connected for prizes\n");
			int coup[5];
			recv(csfd,coup,sizeof(coup),0);
			int f = 1;
			for(int i=0;i<5;i++){
				if(nums[coup[i]] != 1){
					f = 0;
					break;
				}
			}
			if(!f){
				printf("Client not a winner\n");
			}else{
				printf("Client a winner\n");
				if(send_fd(nufd2, csfd) < 0) {
		    		perror("send_fd");
		    		exit(1);
				}
    		}
        }
    }
    return 0;
}
