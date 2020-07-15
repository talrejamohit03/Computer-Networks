#include<unistd.h>
#include<sys/wait.h>
#include<stdio.h>
#include<signal.h>
#include<stdlib.h>


#define MAX 100
int main(){
	system("pidof s > nam.txt");
	
	FILE *fptr = fopen("nam.txt","r");
	
	long pid;
	fscanf(fptr,"%ld",&pid);
	printf("signal sent to %ld",pid);
	kill(pid,SIGUSR1);
	return 0;
}
