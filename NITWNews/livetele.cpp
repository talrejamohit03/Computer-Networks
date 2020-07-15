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
#include<fcntl.h>
using namespace std;

#define MAX 100

int wms;
int pid;
char nam[10];

struct msg_buffer{
	long mtype;
	char mtext[100];
} ms;

int main(){
	pid = getpid();
	cout<<pid<<" live telecast started\n";
	key_t key=ftok("keyfile",67);
	if(key == -1){
		perror("ftok error");
	}
	wms = msgget(key,0666|IPC_CREAT);
	if(wms==-1){
		perror("msgget error");
	}
	
	ms.mtype = pid;
	sprintf(ms.mtext,"%d",pid);
	sprintf(nam,"%d",pid);
	ms.mtext[strlen(ms.mtext)]='\0';
	nam[strlen(nam)]='\0';
	mkfifo(nam,0666);
	if(msgsnd(wms,&ms,sizeof(ms.mtext),0) == -1){
		perror("msgsnd error");
	}
	cout<<"Enter # to terminate\n";
	fflush(stdout);
	while(1){
		char c,buff[MAX];
		int j=0;
		
		read(STDIN_FILENO,&c,1);
		while(c!='\n'){
			buff[j++]=c;
			read(STDIN_FILENO,&c,1);
		}
		buff[j]='\0';
		int wfd = open(nam,O_WRONLY);
		write(wfd,buff,strlen(buff)+1);
		close(wfd);
		if(strcmp(buff,"#")==0)
			break;
		
	}
	//cout<<ms.mtext<<" sent to server\n";
	//free(ms);
	return 0;
}
