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
	while(1){
		int pp[2];
		pipe(pp);
		int infd=dup(0),outfd=dup(1);
	
		dup2(pp[1],1);
		int wfd=fileno(popen("./p2","w"));
		close(pp[1]);
		dup2(outfd,1);
	
		char c,msg[100];
		int j=0;
		read(STDIN_FILENO,&c,1);
		while(c!='\n'){
			msg[j++]=c;
			read(STDIN_FILENO,&c,1);
		}
		msg[j]='\0';
		write(wfd,&msg,strlen(msg));
		close(wfd);
		while(read(pp[0],&c,1))
			cout<<c;
		cout<<"\n";
		close(pp[0]);
	}
	return 0;
}
