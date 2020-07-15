#include<iostream>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>
#include<stdio.h>
using namespace std;

int main(){
	while(1){
		cout<<"Hello from p1\n";
		fflush(stdout);
		sleep(10);
	}
	return 0;
}
