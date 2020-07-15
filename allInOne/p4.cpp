#include<iostream>
#include<unistd.h>
#include<sys/wait.h>
#include<stdio.h>
#include<signal.h>
using namespace std;

#define MAX 100
int main(){
	int ppid = getppid();
	while(1){
		kill(ppid,SIGUSR1);
		sleep(10);
	}
	return 0;
}
