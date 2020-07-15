#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<string.h>
#include<sys/stat.h>


int main(){
	mkfifo("p2fifo",0666);
	int wfd = open("p2fifo",O_WRONLY);
	while(1){
		//for(int i=0;i<0x4fffffff;i++);
		
		char buf[]= "hello from p2";
		write(wfd,buf,strlen(buf)+1);
		sleep(5);
	}
	close(wfd);
	return 0;
}
