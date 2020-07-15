#include<iostream>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<string.h>
#include<sys/stat.h>
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
using namespace std;

void hdfn(int signo){
	exit(0);
}
int main(int argc, char *argv[]){
	char buff[]="hello from echo\n";
	signal(SIGINT,hdfn);
	while(1){
		int wfd = open(argv[1], O_WRONLY);
        write(wfd,buff,strlen(buff)+1);
        close(wfd);
        sleep(5);
	}
	return 0;
}
