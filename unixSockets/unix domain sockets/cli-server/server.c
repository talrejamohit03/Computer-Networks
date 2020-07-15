#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

char *path = "./fsock";

int serv_listen() {
    int fd, len;
    struct sockaddr_un u;

    if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    unlink(path);
    u.sun_family = AF_UNIX;
    strcpy(u.sun_path, path);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(path);
    if(bind(fd, (struct sockaddr*)&u, len)) {
        perror("bind");
        exit(1);
    }

    if(listen(fd, 10) < 0) {
        perror("listen");
        exit(1);
    }

    return fd;
}

int main() {
    struct sockaddr_un cli_addr;
    int len, ufd, nufd;

    len = sizeof(cli_addr);
    ufd = serv_listen();
    if((nufd = accept(ufd, (struct sockaddr*)&cli_addr, &len)) < 0) {
        perror("accept");
        exit(1);
    }

    char buff[100];
    recv(nufd, buff, 99, 0);
    printf("%s\n", buff);
    buff[0] = 'r';
    send(nufd, buff, strlen(buff) + 1, 0);

    unlink(path);
}