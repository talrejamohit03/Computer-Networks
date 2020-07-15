#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include<sys/msg.h>
#include<sys/ipc.h>
using namespace std;

#define MAX 100
long typs[10];
int cnt=0;
int rms,wms;

struct msg_buffer{
	long mtype;
	char mtext[MAX];
} ms;

void hdfn(int signo){
	msgctl(rms,IPC_RMID,NULL);
	msgctl(wms,IPC_RMID,NULL);
	signal(SIGINT,SIG_DFL);
	raise(SIGINT);
}
int main(int argc, char* argv[]){
	//signal(SIGINT,hdfn);
	key_t key=ftok(".",12);
	if(key == -1){
		perror("ftok error");
	}
	rms = msgget(key,0666|IPC_CREAT);
	if(rms==-1){
		perror("msgget error");
	}
	key=ftok(".",10);
	if(key == -1){
		perror("ftok error");
	}
	wms = msgget(key,0666|IPC_CREAT);
	if(wms==-1){
		perror("msgget error");
	}
	cout<<"Server started\n";
	while(1){
		//struct msg_buffer *ms=(struct msg_buffer *)malloc(sizeof(struct msg_buffer));
		if(msgrcv(rms,&ms,sizeof(ms.mtext),0,0) == -1){
			perror("msgrcv error");
		}
		
		//cout<<ms.mtype<<": "<<ms.mtext<<" message received\n";
		if(strcmp(ms.mtext,"|")==0){
			typs[cnt++]=ms.mtype;
			cout<<ms.mtype<<" client added\n";
		}else{
			long curr = ms.mtype;
			for(int j=0;j<cnt;j++){
				if(typs[j]==curr)
					continue;
				ms.mtype = typs[j];
				
				if(msgsnd(wms,&ms,sizeof(ms.mtext),0) == -1){
					perror("msgsnd error");
				}
			}
		}
		//sleep(5);
	}
	
	return 0;
}
