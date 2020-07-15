#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//char *socket_path = "./socket";
char *path = "./usoc";

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

	while((rc=read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
		if (write(usfd, buf, rc) != rc) {
			if(rc > 0) 
				fprintf(stderr,"partial write");
			else{
				perror("write error");
				exit(1);
			}
		}
	}
	return 0;
}
