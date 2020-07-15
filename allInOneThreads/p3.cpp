#include<iostream>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<string.h>
#include<sys/stat.h>
#include<stdio.h>
using namespace std;

#define MAX 100
int main(){
	while(1){
		cout<<"Hello from p3\n";
		fflush(stdout);
		sleep(10);
	}
	return 0;
}
