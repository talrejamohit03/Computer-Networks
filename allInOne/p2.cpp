#include<iostream>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<string.h>
#include<sys/stat.h>
using namespace std;

int main(){
	mkfifo("p2fifo",0666);
	while(1){
		sleep(10);
		int wfd = open("p2fifo",O_WRONLY);
		char buf[]= "hello from p2\n";
		write(wfd,buf,strlen(buf)+1);
		close(wfd);
		
	}
	return 0;
}
