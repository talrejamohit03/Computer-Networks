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
#include <sys/shm.h>
#include <sys/sem.h>
using namespace std;

#define MAX 100
long typs[10];
int cnt=0,nrep=0;
int rms;

struct msg_buffer{
	long mtype;
	char mtext[MAX];
} ms;

key_t key=ftok("keyfile",65);
union semun {
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INFO
                               (Linux-specific) */
};

struct sembuf p = {0,-1,SEM_UNDO}; //wait
struct sembuf v = {0,1,SEM_UNDO}; //signal

struct shmem{
	char news[MAX];
	int livecnt;
};
struct shmem *shm;
int semid,shmid;



int main(int argc, char* argv[]){
	shmid = shmget(key,sizeof(struct shmem),IPC_CREAT|0660);
	shm = (struct shmem*)shmat(shmid,NULL,0);
	shm->news[0]='\0';
	shm->livecnt=0;
	
	key = ftok("keyfile",66);
	union semun u;
	semid = semget(key,2,0666|IPC_CREAT);
	u.val=0;
	semctl(semid,0,SETVAL,u);
	u.val=0;
	semctl(semid,1,SETVAL,u);
	shmid = shmget(key,sizeof(shmem),IPC_CREAT|0660);
	
	key=ftok("keyfile",67);
	if(key == -1){
		perror("ftok error");
	}
	rms = msgget(key,0666|IPC_CREAT);
	if(rms==-1){
		perror("msgget error");
	}
	cout<<"Editor started\n";
	while(1){

		while(shm->livecnt>0){
			cout<<"Live telecast going on\n";
			sleep(5);
		}
		if(msgrcv(rms,&ms,sizeof(ms.mtext),0,0) == -1){
			perror("msgrcv error");
		}
		
		if(ms.mtext[0]=='/' && ms.mtext[1]=='d'){
			cout<<"message passed to document writer\n";
			int wdocfd = fileno(popen("./docwriter","w"));
			char *msg = strstr(ms.mtext,"/d");
			write(wdocfd,msg,strlen(msg));
			close(wdocfd);
		}
		//cout<<ms.mtype<<": "<<ms.mtext<<" message received\n";
		else if(strcmp(ms.mtext,"|")==0){
			typs[cnt++]=ms.mtype;
			cout<<ms.mtype<<" reporter added\n";
		}else{
			strcpy(shm->news,ms.mtext);
			int j=0,lpid=0;
			while(j<strlen(shm->news) && isdigit(shm->news[j]))
				lpid = lpid*10 + (shm->news[j++]-'0');
			if(j == strlen(shm->news)){
				if(nrep % 2 == 0){
					cout<<"n1 going into live telecast\n";
					fflush(stdout);
				}else{
					cout<<"n2 going into live telecast\n";
					fflush(stdout);
				}
				shm->livecnt++;
			}
			if(nrep % 2 == 0){
				v.sem_num = 0;
				semop(semid,&v,1);
			}else{
				v.sem_num = 1;
				semop(semid,&v,1);
			}
			nrep = (nrep + 1) % 2;
		}
		//sleep(5);
		cout<<shm->livecnt<<" are live\n";
	}
	
	return 0;
}
