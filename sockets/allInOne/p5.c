#include<unistd.h>
#include<sys/wait.h>
#include<stdio.h>
#include<string.h>

#define MAX 100

int main(){
	while(1){
		char buff[MAX];
		fgets(buff,MAX,stdin);
		buff[strlen(buff)-1]='\0';
		printf("%s: p5\n",buff);
		fflush(stdout);
	}
	return 0;
}
