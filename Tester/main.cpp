#include <iostream>
#include <pthread.h>
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

int main(){
	int tfd=open("tc.txt",O_RDONLY),ofd=open("o1.txt",O_WRONLY|O_CREAT|O_TRUNC,0660);
	//int infd=dup(0),outfd=dup(1);
	dup2(tfd,0);
	int rfd=fileno(popen("./p","r"));
	char c;
	while(read(rfd,&c,1)){
		write(ofd,&c,1);
	}
	close(rfd);
	close(tfd);
	close(ofd);
	tfd=open("tc.txt",O_RDONLY);
	ofd=open("ostand.txt",O_WRONLY|O_CREAT|O_TRUNC,0660);
	dup2(tfd,0);
	dup2(ofd,1);
	rfd=fileno(popen("./pstand","r"));
	while(read(rfd,&c,1))
		write(ofd,&c,1);
	close(rfd);
	close(tfd);
	close(ofd);
	return 0;
}
