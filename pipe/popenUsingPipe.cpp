#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
using namespace std;

int main(int argc, char *argv[]){
	int pp[2];
	pipe(pp);
	if(fork()>0){
		close(pp[1]);
		wait(NULL);
		char c;
		while(read(pp[0],&c,1))
			cout<<c;
	}else{
		dup2(pp[1],1);
		char *args[]={"./p2",NULL};
		execvp(args[0],args);
	}
	return 0;
}
