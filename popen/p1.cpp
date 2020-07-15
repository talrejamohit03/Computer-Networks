#include<bits/stdc++.h>
#include<fcntl.h>
#include<unistd.h>
using namespace std;

int main(){
	FILE *fp=popen("./p2","w");
	dup2(fileno(fp),1);
	cout<<"FromP1";
	return 0;
}
