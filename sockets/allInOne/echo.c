#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<string.h>
#include<sys/stat.h>
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>

#define MAX 100
int main(int argc, char *argv[]){
	long epid = getpid();
	printf("Echo server pid = %ld\n",epid);
	fflush(stdout);
	int cnt = 0;
	while(cnt < 5){
		cnt++;
		printf("Hello from echo\n");
		fflush(stdout);
		sleep(5);
	}
	kill(getppid(),SIGUSR2);
	return 0;
}
