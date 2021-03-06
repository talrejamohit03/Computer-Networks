#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include<sys/msg.h>
#include<sys/ipc.h>
#include<errno.h>
using namespace std;

#define MAX 100

int wms;
long pid;

struct msg_buffer{
	long mtype;
	char mtext[100];
} ms;
/*
void *reader(void *arg){
	//cout<<"reader started\n";
	while(1){
		//struct msg_buffer *ms=(struct msg_buffer *)malloc(sizeof(msg_buffer));
		struct msg_buffer rmsg;
		if(msgrcv(rms,&rmsg,sizeof(rmsg.mtext),pid,0) == -1){
			perror("msgrcv error");
		}
		cout<<rmsg.mtype<<": "<<rmsg.mtext<<"\n";
		fflush(stdout);
		//free(ms);
		sleep(5);
	}
}*/
void writer(){
	//cout<<"writer started\n";
	while(1){
		//struct msg_buffer *ms=(struct msg_buffer *)malloc(sizeof(struct msg_buffer));
		struct msg_buffer wmsg;
		wmsg.mtype = pid;
		char c;
		int j=0;
		read(STDIN_FILENO,&c,1);
		while(c!='\n'){
			wmsg.mtext[j++]=c;
			read(STDIN_FILENO,&c,1);
		}
		wmsg.mtext[j]='\0';
		
		if(msgsnd(wms,&wmsg,sizeof(wmsg.mtext),0) == -1){
			perror("msgsnd error at write");
		}
		//free(ms);
	}
}
int main(int argc, char* argv[]){
	pid = getpid();
	cout<<pid<<" reporter started\n";
	key_t key=ftok("keyfile",67);
	if(key == -1){
		perror("ftok error");
	}
	wms = msgget(key,0666|IPC_CREAT);
	if(wms==-1){
		perror("msgget error");
	}
	
	ms.mtype= pid;
	ms.mtext[0]='|';
	ms.mtext[1]='\0';
	if(msgsnd(wms,&ms,sizeof(ms.mtext),0) == -1){
		perror("msgsnd error");
	}
	//cout<<ms.mtext<<" sent to server\n";
	//free(ms);
	writer();
	return 0;
}
