#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>
using namespace std;

char *wcfd[10];

int main(int argc, char* argv[]){
	mkfifo("ffifo",0666);
	sleep(10);
	int rfd=open("ffifo",O_RDONLY);
	cout<<"famous fifo opened\n";
	char buff[50];
	cout<<read(rfd,&buff,50)<<" chs read\n";
	cout<<buff;
	return 0;
}
