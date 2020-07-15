#include <iostream>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>
using namespace std;

int rfd,wfd;
int pid;
char pro[20];

void *reader(void *arg){
	//cout<<"reader started\n";
	while(1){
		rfd=open(pro,O_RDONLY);
		char c,msg[500];
		int j=0;
		while(read(rfd,&c,1))
			msg[j++]=c;
		msg[j]='\0';
		cout<<msg<<"\n";
		close(rfd);
	}
}
void *writer(void *arg){
	//cout<<"writer started\n";
	while(1){
		char buff[500],inp[500];
		char c;
		int j=0;
		read(STDIN_FILENO,&c,1);
		while(c!='\n'){
			inp[j++]=c;
			read(STDIN_FILENO,&c,1);
		}
		inp[j]='\0';
		//cout<<inp<<" is the input\n";
		buff[0]='\0';
		strcat(buff,pro);
		strcat(buff,": ");
		strcat(buff,inp);
		//cout<<buff<<"written to ffifo\n";
		wfd=open("ffifo",O_WRONLY);
		write(wfd,&buff,strlen(buff)+1);
		close(wfd);
	}
}
int main(int argc, char* argv[]){
	pid=getpid();
	sprintf(pro,"%d",pid);
	cout<<pro<<" client started\n";
	strcat(pro,"fifo");
	mkfifo(pro,0666);
	char ping[50];
	ping[0]='\0';
	strcat(ping,pro);
	strcat(ping,"|");
	//cout<<ping<<" is going to be written to ffifo\n";
	wfd=open("ffifo",O_WRONLY);
	
	write(wfd,&ping,strlen(ping)+1);
	close(wfd);
	//cout<<fstr<<" is the ping written to ffifo\n";
	
	pthread_t rth,wth;
	pthread_create(&wth,NULL,writer,NULL);
	pthread_create(&rth,NULL,reader,NULL);
	pthread_join(wth,NULL);
	pthread_join(rth,NULL);
	return 0;
}
