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

int cnt=0,pids[10];
char wcfd[20][20];

int search(int pid){
	for(int i=0;i<cnt;i++){
		if(pids[i]==pid)
			return i;
	}
	return -1;
}
int main(int argc, char* argv[]){
	mkfifo("ffifo",0666);
	cout<<"Chat Server started\n";
	while(1){
		int rfd=open("ffifo",O_RDONLY);

		char msg[500];
		int cpid=0,j=0;
		/*
		read(rfd,&msg,500);
		close(rfd);
		cout<<msg<<" received\n";
		*/
		char c;
		while(read(rfd,&c,1))
			msg[j++]=c;
		close(rfd);
		msg[j]='\0';
		j=0;
		while(isdigit(msg[j]))
			cpid=10*cpid+(msg[j++]-'0');
		//cout<<cpid<<" sent the message: "<<msg<<"\n";
		if(msg[strlen(msg)-1]=='|'){
			strncpy(wcfd[cnt],msg,strlen(msg)-1);	
			pids[cnt]=cpid;
			cnt++;
			cout<<"Entry for client "<<wcfd[cnt-1]<<" created\n";
			cout<<cnt<<" total clients\n";
		}else{
			int sendingclient=search(cpid);
			//cout<<"entry found at "<<sendingclient<<"\n";
			for(int i=0;i<cnt;i++){
				if(i==sendingclient)
					continue;
				int wfd=open(wcfd[i],O_WRONLY);
				write(wfd,&msg,strlen(msg));
				close(wfd);
			}
		}
	}
	return 0;
}
