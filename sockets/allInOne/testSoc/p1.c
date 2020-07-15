#include<unistd.h>
#include<sys/wait.h>
#include<string.h>
#include<stdio.h>

int main(){
	printf("Hello from p1\n");
	fflush(stdout);
	while(1);
	return 0;
}
