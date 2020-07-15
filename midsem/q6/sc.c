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

void swap(int *c1, int *c2){
	int tmp = *c1;
	*c1 = *c2;
	*c2 = tmp;
}
int main(){
	int f=1, c1 = 0, c2 = 1;
	while(f){
		char buff[MAX];
		recv(c1,buff,MAX,0);
		//printf("%s\n",buff);
		send(c2,buff,strlen(buff)+1,0);
		recv(c2,buff,MAX,0);
		send(c1,buff,strlen(buff)+1,0);
		recv(c1,buff,MAX,0);
		if(strcmp(buff,"+\n") == 0){
			swap(&c1,&c2);
		}else{
			c1 = 0;
			c2 = 1;
		}
	}
	return 0;
}
