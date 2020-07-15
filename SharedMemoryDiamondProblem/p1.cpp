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

key_t key=ftok(".",65);
union semun {
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INFO
                               (Linux-specific) */
};


struct sembuf p = {0,-1,SEM_UNDO}; //wait
struct sembuf v = {0,1,SEM_UNDO}; //signal
struct shmem *x,*y;
int semid,shmid1,shmid2;

struct shmem{
	int i;
};

//execute p2 first
int main(){
	shmid1 = shmget(key,sizeof(shmem),IPC_CREAT|0660);
	key = ftok(".",66);
	shmid2 = shmget(key,sizeof(shmem),IPC_CREAT|0660);
	
	x = (struct shmem*)shmat(shmid1,NULL,0);
	y = (struct shmem*)shmat(shmid2,NULL,0);
	x->i = 0;
	y->i = 0;
	union semun u;
	key = ftok(".",67);
	semid = semget(key,2,0666|IPC_CREAT);
	u.val=0;
	semctl(semid,0,SETVAL,u);
	u.val=0;
	semctl(semid,1,SETVAL,u);
	while(1){
		int tmp=y->i;
		cout<<tmp<<" read from shm y\n";
		tmp++;
		x->i=tmp;
		cout<<tmp<<" after compute by p1, written to shm x\n";
		sleep(1);
		v.sem_num=0;
		semop(semid,&v,1);
		p.sem_num=1;
		semop(semid,&p,1);
		
		sleep(1);
	}
	
	return 0;
}
