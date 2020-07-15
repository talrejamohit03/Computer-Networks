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
		strcpy(buff,"");
		fflush(stdin);
		fgets(buff,MAX,stdin);
		buff[strlen(buff)-1] = '\0';
		if(!strcmp(buff,"#")){
			f=0;
		}
		strcat(buff," S1 processed it");
		printf("%s\n",buff);
		fflush(stdout);
	}
	printf("S1 ended\n");
	return 0;
}
