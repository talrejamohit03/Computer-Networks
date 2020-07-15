#include<stdio.h>
#include<unistd.h>
#include<signal.h>
#include<string.h>
#include<stdlib.h>

int main(){
	system("pgrep p1 > nam.txt");
	
	FILE *fptr = fopen("nam.txt","r");
	
	long pid;
	fscanf(fptr,"%ld",&pid);
	
	kill(pid,SIGUSR1);
	return 0;
}
