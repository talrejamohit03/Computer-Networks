#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<string.h>
#include<sys/stat.h>
#include<stdio.h>


#define MAX 100
int main(){
	while(1){
		printf("Hello from p3\n");
		fflush(stdout);
		//for(int i=0;i<0x4fffffff;i++);
		sleep(5);
	}
	return 0;
}
