#include<stdio.h>
#include<unistd.h>
#include<signal.h>
#include<string.h>

void hdfn(int sig, siginfo_t *siginfo, void *context){
	printf("Sending pid: %ld, UID: %ld\n",(long) siginfo->si_pid, (long) siginfo->si_uid);
}

int main(){
	int pid = getpid();
	printf("PID of P1: %d\n",pid);
	struct sigaction act;
	
	act.sa_sigaction = &hdfn;
	act.sa_flags = SA_SIGINFO;
	
	if(sigaction(SIGUSR1, &act, NULL) < 0){
		perror("sigaction");
		return 1;
	}
	while(1){
		sleep(5);
	}
	return 0;
}
