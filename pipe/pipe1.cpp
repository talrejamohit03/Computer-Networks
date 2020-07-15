#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
using namespace std;

int main(int argc, char *argv[]){
	int pp1[2],pp2[2];
	pipe(pp1);
	pipe(pp2);
	if(fork()>0){
		close(pp1[0]);
		close(pp2[1]);
		write(pp1[1],argv[1],strlen(argv[1]));
		close(pp1[1]);
		cout<<"file name written to pipe 1\n";
		wait(NULL);
		char c;
		cout<<"parent waited for child and is now reading from pipe 2\n";
		while(read(pp2[0],&c,1)!=0){
			cout<<c;
		}
	}else{
		close(pp1[1]);
		close(pp2[0]);
		char c;
		char fname[100];
		int i=0;
		while(read(pp1[0],&c,1) > 0){
			fname[i++]=c;
		}
		cout<<"filename read by child process from pipe 1 and now reading file, sending to parent through pipe 2 \n";
		fname[i]='\0';
		int fd=open(fname,O_RDONLY);
		while(read(fd,&c,1)!=0){
			write(pp2[1],&c,1);
		}
		close(pp2[1]);
	}
	return 0;
}
