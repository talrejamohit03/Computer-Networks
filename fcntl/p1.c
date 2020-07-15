#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int pr = 1;

void hdfn(int x)
{
    printf("Recieved signal from pm\n");
    pr--;
}

int main()
{
    signal(SIGUSR1, hdfn);
    printf("P1 is running\n");
    while(pr);
    return 0;
}
