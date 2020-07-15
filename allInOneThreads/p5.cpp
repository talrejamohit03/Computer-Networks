#include<iostream>
#include<unistd.h>
#include<sys/wait.h>
#include<stdio.h>
#include<string.h>
using namespace std;

#define MAX 100

int main(){
	while(1){
		string inp;
		getline(cin,inp);
		cout<<inp<<": p5\n";
		fflush(stdout);
	}
	return 0;
}
