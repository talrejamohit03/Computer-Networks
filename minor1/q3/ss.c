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
int sfd,nsfds[MAX], cnt = 0;
struct sockaddr_in serv_addr, cli_addr, cli_addr_list[MAX];

int main(int argc, char **argv) {
    
	printf("Special server %s started\n",argv[1]);
	while(1) {
		struct pollfd fds[cnt+1];

		fds[0].fd = 0;
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
			if((nsfds[cnt] = accept(0, (struct sockaddr *)&cli_addr, &clilen)) < 0) {
				perror("accept");
				return 1;
			}
			if(cnt == 3){
				kill(getppid(),SIGUSR1);
				break;
			}
			printf("new client added to %s\n",argv[1]);

			bcopy((char *)&cli_addr,(char *)&cli_addr_list[cnt],sizeof(cli_addr));

			cnt++;
			continue;
		}

		for(int i = 0; i < cnt; i++) {
			if(fds[i + 1].revents & POLLIN) {
				char buff[MAX];
				recv(fds[i + 1].fd, buff, MAX,0);
				strcat(buff,"from special server ");
				strcat(buff,argv[1]);
				send(fds[i + 1].fd, buff,strlen(buff)+1,0);
				printf("%s written to client\n",buff);
			}
		}
	}
    return 0;
}
