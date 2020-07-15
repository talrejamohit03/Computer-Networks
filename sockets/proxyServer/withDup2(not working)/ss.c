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
int sfd, nsfds[MAX], portno, sno, cnt = 0;
struct sockaddr_in serv_addr, cli_addr, cli_addr_list[MAX];

int main() {
	printf("Enter server number: \n");
	scanf("%d",&sno);
	printf("Enter port number: \n");
	scanf("%d", &portno);
    //initialize server address
    memset(&serv_addr, 0, sizeof(serv_addr));
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
    char snostr[5];
    sprintf(snostr,"%d",sno);
    char msgcat[] = " hello from special server ";
    strcat(msgcat,snostr);
	while(1) {
		struct pollfd fds[cnt+1];

		fds[0].fd = sfd;
		fds[0].events = POLLIN;
		fds[0].revents = 0;
		for(int i = 0; i < cnt; i++) {
			fds[i + 1].fd = nsfds[i];
			fds[i + 1].events = POLLIN;
			fds[i+1].revents = 0;
		}
		poll(fds, cnt + 1, -1);

		if(fds[0].revents & POLLIN) {
			int clilen = 0;
			if((nsfds[cnt] = accept(sfd, (struct sockaddr *)&cli_addr, &clilen)) < 0) {
				perror("accept");
				return 1;
			}

			printf("new client added\n");

			bcopy((char *)&cli_addr,(char *)&cli_addr_list[cnt],sizeof(cli_addr));

			cnt++;
			continue;
		}

		for(int i = 0; i < cnt; i++) {
			if(fds[i + 1].revents & POLLIN) {
				char buff[MAX];
				recv(fds[i + 1].fd, buff, MAX,0);
				strcat(buff,msgcat);
				send(fds[i + 1].fd, buff,strlen(buff)+1,0);
			}
		}
	}
    return 0;
}
