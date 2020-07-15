#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

char *path = "./fsock";

int cli_conn(char *name) {
    int fd, len;
    struct sockaddr_un u, su;

    if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    unlink(name);
    u.sun_family = AF_UNIX;
    strcpy(u.sun_path, name);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(name);
    if(bind(fd, (struct sockaddr*)&u, len)) {
        perror("bind");
        exit(1);
    }

    su.sun_family = AF_UNIX;
    strcpy(su.sun_path, path);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(path);
    if(connect(fd, (struct sockaddr*)&su, len) < 0) {
        exit(1);
    }

    return fd;
}

int main() {
    struct sockaddr_un cli_addr;
    int len, ufd;
    char *name = "./csock";

    ufd = cli_conn(name);

    char buff[100] = "test";
    send(ufd, buff, strlen(buff) + 1, 0);
    recv(ufd, buff, 99, 0);
    printf("%s\n", buff);

    unlink(name);
}