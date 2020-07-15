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
#include <sys/shm.h>
#include <sys/sem.h>
using namespace std;

#define MAX 100
key_t key=ftok("keyfile",65);

struct sembuf p = {0,-1,SEM_UNDO}; //wait
struct sembuf v = {0,1,SEM_UNDO}; //signal
struct shmem *shm;
int shmid,semid;

struct shmem{
	char news[MAX];
	int livecnt;
};

int main(){
	cout<<"N2\n";
	shmid = shmget(key,sizeof(struct shmem),IPC_CREAT|0660);
	shm = (struct shmem*)shmat(shmid,NULL,0);
	//shm->news[0]='\0';
	key = ftok("keyfile",66);
	semid = semget(key,2,0666|IPC_CREAT);
	
	mkfifo("screenfifo",0666);
	while(1){
		p.sem_num = 1;
		semop(semid,&p,1);
		int j=0,lpid=0;
		while(j<strlen(shm->news) && isdigit(shm->news[j]))
			lpid = lpid*10 + (shm->news[j++]-'0');
		if(j == strlen(shm->news)){
			cout<<"Live telecast started by: "<<shm->news<<"\n";
			while(1){
				int rfd = open(shm->news,O_RDONLY);
				char c,msg[MAX];
				int j=0;
				while(read(rfd,&c,1))
					msg[j++]=c;
				msg[j]='\0';
				if(strcmp(msg,"#")==0)
					break;
				int wfd = open("screenfifo",O_WRONLY);
				write(wfd,msg,strlen(msg)+1);
				close(wfd);
			}
			cout<<"Live telecast ended\n";
			shm->livecnt--;
		}else{
			int wfd = open("screenfifo",O_WRONLY);
			char buf[MAX];
			buf[0]='\0';
			strcat(buf,"n2: ");
			strcat(buf,shm->news);
			write(wfd,buf,strlen(buf)+1);
			close(wfd);
		}
	}
	
	return 0;
}
