#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <poll.h>
#include <errno.h>

struct msg {
    char u_name[100];
    char data[256];
};

char p2fo[] = "p2fo";
int cs = 0, p1[2];
int connected[100] = {0}, echo_ids[100] = {0};
int saved_stdin, saved_stdout;
int sfd, nsfds[100], portno = 51720;
struct sockaddr_in serv_addr, cli_addr, cli_addr_list[100];
struct msg buff;
fd_set readfds;

//termination
void hdfn(int signo) {
    unlink(p2fo);
    signal(SIGINT, SIG_DFL);
    raise(SIGINT);
}

void hdfn2(int signo) {
    for(int i = 0; i < cs; i++) {
        if(connected[i] == 1) {
            int c = fork();
            if(c == 0) {
                dup2(nsfds[i], 1);
                char *args[] = {NULL};
                if(execvp("./echo", args) == -1) {
                    perror("exec echo\n");
                }
            }
            connected[i] = 0;
            FD_CLR(nsfds[i], &readfds);
            echo_ids[i] = c;
            break;
        }
    }
}

void hdfn3(int signo, siginfo_t *siginfo, void* context) {
    long s_pid = siginfo->si_pid;
    for(int i = 0; i < cs; i++) {
        if(echo_ids[i] == s_pid) {
            echo_ids[i] = 0;
            connected[i] = 1;
            break;
        }
    }
}

void sendAll(int fd) {
    for(int i = 0; i < cs; i++) {
        if(connected[i] && nsfds[i] != fd) {
            send(nsfds[i], &buff, sizeof(buff), 0);
        }
    }
}

int main() {
    saved_stdin = dup(0);
    saved_stdout = dup(1);

    signal(SIGINT, hdfn);
    signal(SIGUSR1, hdfn2);

    struct sigaction act;
    memset (&act, '\0', sizeof(act));
    act.sa_sigaction = &hdfn3;
    act.sa_flags = SA_SIGINFO;
    if (sigaction(SIGUSR2, &act, NULL) < 0)
		perror ("sigaction");
    
    int c;
    //start p4
    c = fork();
    if(c == 0) {
        char *args[] = {NULL};
        if(execvp("./p4", args) == -1) {
            perror("exec p4\n");
        }
    }

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

    while(1) {
        int maxfd = sfd;
        FD_ZERO(&readfds);
        FD_SET(sfd, &readfds);

        for(int i = 0; i < cs; i++) {
            if(connected[i]) {
                FD_SET(nsfds[i], &readfds);
                if(maxfd < nsfds[i]) {
                    maxfd = nsfds[i];
                }
            }
        }
        
        int nr = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if(nr < 0) {       
            perror("");
            continue;
        }
        if(FD_ISSET(sfd, &readfds)) {
            int len = sizeof(cli_addr_list[cs]);
            nsfds[cs] = accept(sfd, (struct sockaddr*)&cli_addr_list[cs], &len);
            if(nsfds[cs] > 0) {
                char buff1[100], temp[100];
                sprintf(temp, "%d", cli_addr_list[cs].sin_addr.s_addr);
                inet_ntop(AF_INET, temp, buff1, sizeof(buff1));
                printf("Connected to client %s::%d\n", buff1, ntohs(cli_addr_list[cs].sin_port));
                connected[cs] = 1;
                cs++;
            }
            else {
                perror("accept");
            }
            continue;
        }
        for(int i = 0; i < cs; i++) {
            if(connected[i] && FD_ISSET(nsfds[i], &readfds)) {
                int r = recv(nsfds[i], &buff, sizeof(buff), 0);
                if(r == 0) {
                    connected[i] = 0;
                }
                else {
                    sendAll(nsfds[i]);
                }
            }
        }
    }
}