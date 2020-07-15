#include<bits/stdc++.h>
#include<fcntl.h>
#include<unistd.h>
using namespace std;

int main(){
	int tmpfd=dup(1);
	FILE *fp=popen("./p2","w");
	dup2(fileno(fp),1);
	cout<<"FromP1";
	fflush(stdout);
	//pclose(fp);
	dup2(tmpfd,1);
	cout<<"Output from p1 restored to stdout";
	return 0;
}
