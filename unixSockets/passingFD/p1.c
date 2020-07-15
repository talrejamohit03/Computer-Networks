#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define CONTROLLEN CMSG_LEN(sizeof(int))
static struct cmsghdr *cmptr = NULL;

char *path = "./usoc";

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
	char buf[100];
	int usfd,cl,rc,nufd;

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
	struct sockaddr cli_addr;
	int len;
	if((nufd = accept(usfd, (struct sockaddr*)&cli_addr, &len)) < 0) {
	    perror("accept");
	    exit(1);
	}
	printf("Client connected\n");
	int fd1 = open("tmp.txt", O_RDONLY);

    if(send_fd(nufd, fd1) < 0) {
        perror("send_fd");
        exit(1);
    }
	
	return 0;
}
