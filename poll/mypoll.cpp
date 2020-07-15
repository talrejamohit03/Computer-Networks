#include<stdio.h>
#include<poll.h>
#include<iostream>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
using namespace std;

#define TIMEOUT 5
int main(){
	int rfd=open("text.txt",O_RDONLY);
	
	struct pollfd pfd[2];
	pfd[0].fd=STDIN_FILENO;
	pfd[0].events = 0;
	pfd[0].events |= POLLIN;
	pfd[0].revents=0;
	pfd[1].fd=rfd;
	pfd[1].events = 0;
	pfd[1].events |= POLLIN;
	pfd[1].revents=0;
	if(poll(pfd,2,TIMEOUT*1000)==0){
		cout<<"time elapsed\n";
		return 0;
	}
	if(pfd[1].revents & POLLIN){
		char buf[100];
		read(rfd,buf,100);
		cout<<buf<<" read from file \n";
	}
	if(pfd[0].revents & POLLIN){
		string s;
		cin>>s;
		cout<<s<<" read from stdin\n";
	}
	return 0;
}
