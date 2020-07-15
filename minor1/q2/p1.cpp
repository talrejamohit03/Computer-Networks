#include<bits/stdc++.h>
#include<fcntl.h>
#include<unistd.h>
#include<poll.h>
using namespace std;

int main(int argc, char *argv[]){
	int rfd[3], wfd;
	rfd[0] = fileno(popen("./p2","r"));
	rfd[1] = fileno(popen("./p3","r"));
	rfd[2] = fileno(popen("./p4","r"));
	
	wfd = fileno(popen("./p5","w"));
	dup2(wfd,1);
	
	struct pollfd pfds[3];
	
	for(int i=0;i<3;i++){
		pfds[i].fd = rfd[i];
		pfds[i].events = POLLIN;
	}
	while(1){
		poll(pfds,3,-1);
		for(int i=0;i<3;i++){
			if(pfds[i].revents & POLLIN){
				dup2(pfds[i].fd,0);
				string s;
				cin>>s;
				cout<<s<<"\n";
				fflush(stdout);
			}
		}
	}
	
	return 0;
}
