#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>

int main() {
    printf("Press 1 to send signal\n");
    while(1) {
        int x;
        scanf("%d", &x);
        if(x == 1) {
            kill(getppid(), SIGUSR1);
        }
    }
}