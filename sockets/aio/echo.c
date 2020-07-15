#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

struct msg {
    char u_name[100]; //username
    char data[256]; //message content
};

int main(int argc, char *argv[]) {
    struct msg buff;
    buff.u_name[0]= '\0';
    sprintf(buff.data, "%d", getpid());
    strcat(buff.data, " is sending");
    sleep(1);
    for(int i = 0; i < 2; i++) {
        write(1, &buff, sizeof(buff));
        sleep(2);
    }
    kill(getppid(), SIGUSR2);
}