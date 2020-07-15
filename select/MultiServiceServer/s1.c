#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <poll.h>
#include <signal.h>
#include <fcntl.h>

#define MAX 100

int main(){
	printf("Enter # to terminate\n");
	fflush(stdout);
	int f=1;
	while(f){
		char buff[MAX];
		fgets(buff,MAX,stdin);
		if(!strcmp(buff,"#\n")){
			f=0;
		}
		strcat(buff," S1 processed it");
		printf("%s\n",buff);
		fflush(stdout);
	}
	printf("S1 ended\n");
	return 0;
}
