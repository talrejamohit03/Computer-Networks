#include<unistd.h>
#include<iostream>
#include<wait.h>
#include<signal.h>
using namespace std;

void hdfn(int signo){
	if(signo == SIGUSR1){
		cout<<"Signal from parent\n";
	}else if(signo == SIGUSR2){
		cout<<"Signal from child\n";
	}
}
int main(){
	
	
	int c=fork();
	if(c>0){
		signal(SIGUSR2,hdfn);
		wait(NULL);
//		sleep(1);
		kill(c,SIGUSR1);
		
	}else{
		signal(SIGUSR1,hdfn);
//		sleep(1);
		kill(getppid(),SIGUSR2);
	}

	return 0;
}

