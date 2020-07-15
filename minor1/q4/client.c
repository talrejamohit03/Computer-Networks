#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <poll.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/msg.h>

int rfd, wfd, msgqid;

struct msg {
    int type;
    char buff[1];
};

void hdfn(int signo) {
    char buff[256];
    for(int i = 0; i < 2; i++) {
        fgets(buff, 255, stdin);
        buff[(int)strlen(buff) - 1] = '\0';
        wfd = open("ffo1", O_WRONLY);
        write(wfd, buff, strlen(buff) + 1);
        close(wfd);
        rfd = open("ffo2", O_RDONLY);
        read(rfd, buff, strlen(buff) + 1);
        close(rfd);
        printf("%s\n", buff);
    }
    struct msg m;
    msgrcv(msgqid, &m, sizeof(m), 0, 0);
    printf("Giving turn to : %d\n", m.type);
    kill(m.type, SIGUSR1);
    m.type = getpid();
    msgsnd(msgqid, &m, sizeof(m), 0);
}

int main() {
    printf("pid : %d\n", getpid());
    signal(SIGUSR1, hdfn);
    key_t k = ftok("msgq", 65);
    msgqid = msgget(k, 0666 | IPC_CREAT);
    struct msg m;
    m.type = getpid();
    msgsnd(msgqid, &m, sizeof(m), 0);
    while(1);
}