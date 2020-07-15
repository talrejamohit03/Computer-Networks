#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>

char *path = "./usoc";

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
	while ( (rc=read(nufd,buf,sizeof(buf))) > 0) {
	  printf("read %u bytes: %.*s\n", rc, rc, buf);
	}
	
	return 0;
}
