/*
Author: mohit_talreja
NITW, CSE
*/

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

#define MAX 100

int main(){
	cout<<"Screen\n";
	mkfifo("screenfifo",0666);
	
	while(1){
		int rfd = open("screenfifo",O_RDONLY);
		char c,msg[MAX];
		int j=0;
		while(read(rfd,&c,1))
			msg[j++]=c;
		msg[j]='\0';
		cout<<msg<<"\n";
		fflush(stdout);
	}
	
	return 0;
}
