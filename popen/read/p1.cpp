#include<bits/stdc++.h>
#include<fcntl.h>
#include<unistd.h>
using namespace std;

int main(){
	int tmpfd=dup(0);
	FILE *fp=popen("./p2","r");
	dup2(fileno(fp),0);
	string s;
	cin>>s;
	cout<<"P1 prints "<<s<<"\n";
	//fflush(stdout);
	dup2(tmpfd,0);
	cout<<"Input from p1 restored to stdin\n";
	cin>>s;
	cout<<"Entered text: "<<s;
	return 0;
}
