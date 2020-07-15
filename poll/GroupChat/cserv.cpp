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
#include<poll.h>

#define TIMEOUT 10
using namespace std;

int cnt=0,pids[10][2],gcnt=0;
char wcfd[20][20],gcfd[20][20];

int search(int pid){
	for(int i=0;i<cnt;i++){
		if(pids[i][0]==pid)
			return i;
	}
	return -1;
}
int gsearch(char grp[]){
	for(int i=0;i<gcnt;i++){
		if(strcmp(grp,gcfd[i])==0)
			return i;
	}
	return -1;
}
void readinBuf(char buf[],int fd){
	int j=0;
	char c;
	while(read(fd,&c,1))
		buf[j++]=c;
	buf[j]='\0';
}
int main(int argc, char* argv[]){
	mkfifo("ffifo",0666);//ffifo will only read the pings
	cout<<"Group Chat Server started\n";
	
	while(1){
		int rfd=open("ffifo",O_RDONLY|O_NONBLOCK);
		struct pollfd pfds[gcnt+1];
		pfds[0].fd=rfd;
		pfds[0].events=POLLIN;
		pfds[0].revents=0;
		if(gcnt>0){
			for(int i=0;i<gcnt;i++){
				pfds[i+1].fd=open(gcfd[i],O_RDONLY|O_NONBLOCK);
				pfds[i+1].events=POLLIN;
				pfds[i+1].revents=0;
			}
		}
		poll(pfds,gcnt+1,-1);
		for(int i=1;i<gcnt+1;i++){
			if(pfds[i].revents & POLLIN){
				//cout<<"Something written to group\n";
				char msg[500];
				readinBuf(msg,pfds[i].fd);
				close(pfds[i].fd);
				int cpid=0,j=0;
				while(isdigit(msg[j]))
					cpid=10*cpid+(msg[j++]-'0');
				//cout<<cpid<<" sent the message: "<<msg<<"\n";
				int sendingclient=search(cpid);
				//cout<<"Message being sent to clients of group "<<pids[sendingclient][1]<<"\n";
				for(int j=0;j<cnt;j++){
					if(j!=sendingclient && pids[j][1]==pids[sendingclient][1]){
						//cout<<"Message sent to "<<wcfd[j]<<"\n";
						int wfd=open(wcfd[j],O_WRONLY);
						write(wfd,&msg,strlen(msg));
						close(wfd);
					}
				}
			}
		}
		if(pfds[0].revents & POLLIN){
			char msg[500];
			readinBuf(msg,rfd);
			close(rfd);
			int cpid=0,j=0;
			while(isdigit(msg[j]))
				cpid=10*cpid+(msg[j++]-'0');
			strncpy(wcfd[cnt],msg,j);
			pids[cnt][0]=cpid;
			cout<<"Entry for client "<<pids[cnt][0]<<" created\n";
			j++;
			char cgrp[10];
			int k=0;
			while(j<strlen(msg))
				cgrp[k++]=msg[j++];
			cgrp[k]='\0';
			int g=gsearch(cgrp);
			if(g!=-1){
				pids[cnt][1]=g;
			}else{
				strcpy(gcfd[gcnt],cgrp);
				pids[cnt][1]=gcnt;
				gcnt++;
			}
			cout<<"Group of the client is "<<pids[cnt][1]<<"\n";
			cnt++;
			cout<<cnt<<" total clients\n";
		}
		
		

	}
	return 0;
}
