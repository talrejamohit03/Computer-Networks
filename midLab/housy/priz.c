#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<poll.h>

#define CONTROLLEN CMSG_LEN(sizeof(int))
static struct cmsghdr *cmptr = NULL;

//char *socket_path = "./socket";
char *path = "./usoc2";

int recv_fd(int usfd) {

    struct iovec iov[1];
    char buf[2];
    int newfd;

    struct msghdr msg;

    iov[0].iov_base = buf;
    iov[0].iov_len = sizeof(buf);
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    cmptr = malloc(CONTROLLEN);
    msg.msg_control = cmptr;
    msg.msg_controllen = CONTROLLEN;

    if(recvmsg(usfd, &msg, 0) < 0) {
        perror("recvmsg");
        return -1;
    }

    newfd = *(int *)CMSG_DATA(cmptr);

    return newfd;
}

int main(int argc, char *argv[]) {
	struct sockaddr_un addr;
	char buf[100];
	int usfd,rc;

	if((usfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket error");
		exit(1);
	}
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, path);
	if(connect(usfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		perror("connect error");
		exit(1);
	}
	
    while(1) {
        struct pollfd fds[1];

        fds[0].fd = usfd;
        fds[0].events = POLLIN;
        
        poll(fds,1,-1);
        if(fds[0].revents & POLLIN){
        	printf("New client connected to prize giver\n");
        	int rfd;
        	if((rfd = recv_fd(usfd)) < 0) {
				perror("recv_fd");
				exit(1);
			}
			char buff[] = "your prize";
			send(rfd,buff,sizeof(buff),0);
        }
        
    }
	return 0;
}
