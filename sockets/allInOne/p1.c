#include<unistd.h>
#include<sys/wait.h>
#include<string.h>
#include<stdio.h>

int main(){
	while(1){
		printf("Hello from p1\n");
		fflush(stdout);
		//for(int i=0;i<0x4fffffff;i++);
		sleep(5);
	}
	return 0;
}
