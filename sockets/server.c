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

//msg structure
struct msg {
    char u_name[100]; //username
    char data[256]; //message content
};

//all the vars needed
int sfd, nsfds[100], portno = 51720, cs = 0;
struct sockaddr_in serv_addr, cli_addr, cli_addr_list[100];
struct msg buff;

//handler function to close opened fds when signalled
void hdfn(int signo) {
    close(sfd);
    for(int i = 0; i < cs; i++) {
        close(nsfds[i]);
    }
    signal(SIGINT, SIG_DFL);
    raise(SIGINT);
}

//accept a new client
void acceptor() {
    int clilen = 0;
    if((nsfds[cs] = accept(sfd, (struct sockaddr *)&cli_addr, &clilen)) < 0) {
        perror("accept");
        return;
    }

    //conformation msg to client
    strcpy(buff.data, "connected");
    write(nsfds[cs], &buff, sizeof(buff));

    bcopy((char *)&cli_addr,
            (char *)&cli_addr_list[cs],
            sizeof(cli_addr));
    
    cs++;
}

//send msg to all clients
void sendAll(int curr) {
    for(int i = 0; i < cs; i++) {
        if(i != curr) {
            write(nsfds[i], &buff, sizeof(buff));
        }
    }
}

int main() {

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
        struct pollfd fds[cs + 1];

        fds[0].fd = sfd;
        fds[0].events = POLLIN;

        for(int i = 0; i < cs; i++) {
            fds[i + 1].fd = nsfds[i];
            fds[i + 1].events = POLLIN;
        }

        int p = poll(fds, cs + 1, -1);
        
        if(fds[0].revents & POLLIN) {
            acceptor();
            continue;//continue is important
        }

        for(int i = 0; i < cs; i++) {
            if(fds[i + 1].revents & POLLIN) {
                read(fds[i + 1].fd, &buff, sizeof(buff));
                sendAll(i);
            }
        }
    }
}