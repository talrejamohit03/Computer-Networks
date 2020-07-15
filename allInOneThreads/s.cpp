#include<iostream>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<string.h>
#include<sys/stat.h>
#include<stdio.h>
#include<poll.h>
#include<stdlib.h>
#include<pthread.h>
using namespace std;

#define MAX 100

int cnt=0,pids[10];
int active[10]={0};
char wcfd[20][20]; //nsfds as in the question
pthread_t tids[10];

struct pchild{
	int ch;
	int i;
};
void *tRunner(void *arg){
	waitpid(((struct pchild *)arg)->ch,NULL,0);
	cout<<"call returned to server, client "<<pids[((struct pchild *)arg)->i]<<" joined back\n";
	fflush(stdout);
	active[((struct pchild *)arg)->i]=1;
}

int search(int pid){
	for(int i=0;i<cnt;i++){
		if(pids[i]==pid)
			return i;
	}
	return -1;
}
void initPollFd(struct pollfd *pfd,int ifd,int event){
	pfd->fd=ifd;
	pfd->events=event;
	pfd->revents=0;
}

void sendData(char buff[]){
	//cout<<buff<<" being written to all\n";
	for(int i=0;i<cnt;i++){
		//cout<<buff<<" being written to "<<cnt<<" \n";
		//fflush(stdout);
		if(active[i]==1){
			int wfd=open(wcfd[i],O_WRONLY);
			write(wfd,buff,strlen(buff)+1);
			close(wfd);
		}
	}
	cout<<buff<<"\n";
	fflush(stdout);
}

void hdfn(int signo){
	if(signo == SIGUSR1){
		cout<<"Received signal from p4\n";
		fflush(stdout);
		if(cnt<=0){
			cout<<"No clients available\n";
			fflush(stdout);
			return;
		}
		int i=0;
		for(i;i<cnt;i++){
			if(active[i]==1){
				break;
			}
		}
		if(i == cnt){
			cout<<"All clients already attached to echo\n";
			fflush(stdout);
			return;
		}
		cout<<"Client "<<pids[i]<<" being transferred to echo server\n";
		fflush(stdout);
		active[i]=0;
		/*
		char command[100];
		command[0]='\0';
		strcat(command,"gnome-terminal --working-directory=/home/mohit/Desktop/allInOne -e \"./echo ");
		strcat(command,wcfd[i]);
		strcat(command,"\"");
		*/
		int ch = fork();
		if(ch>0){
			struct pchild *pch=(struct pchild *)malloc(sizeof(struct pchild));
			pch->i=i;
			pch->ch=ch;
			pthread_create(&tids[i],NULL,tRunner,(void *)pch);
			//pthread_join(tids[i],NULL);
		}else{
			char *argv[]={"./echo",wcfd[i],NULL};
			execvp(argv[0],argv);
		}
		/*
		if(ch==0){
			system(command);
			wait(NULL);
			//exit(0);
			//char *argv[]={"./echo",wcfd[i],NULL};
			//execvp(argv[0],argv);
		}else{
			/*
			struct pchild *pch=(struct pchild *)malloc(sizeof(struct pchild));
			pch->i=i;
			pch->ch=ch;
			pthread_create(&tids[i],NULL,tRunner,NULL);
			pthread_join(tids[i],NULL);
			cout<<"call returned to server, client "<<pids[i]<<" joined back\n";
			fflush(stdout);
			active[i]=1;
		}
		*/
		
	}
}

int main(){
	
	mkfifo("p2fifo",0666);
	mkfifo("ffifo",0666);
	signal(SIGUSR1,hdfn);
	cout<<"Server started\n";
	fflush(stdout);

	//for process p5
	int p5fd = fileno(popen("./p5","w"));
	dup2(p5fd,1);
	
	//for process p1
	int pp[2];
	pipe(pp);
	if(fork()>0){
		close(pp[1]);
	}else{
		close(pp[0]);
		dup2(pp[1],1);
		char *argv[]={"./p1",NULL};
		execvp(argv[0],argv);
	}
	//cout<<"p1 executed successfully\n";
	//for process p2
	if(fork()==0){
		char *argv[]={"./p2",NULL};
		execvp(argv[0],argv);
	}
	int ffd = open("p2fifo",O_RDONLY|O_NONBLOCK);
	//cout<<"p2 executed successfully\n";
	//for process p3
	int fd = fileno(popen("./p3","r"));
	//cout<<"p3 executed successfully\n";
	//for process p4
	if(fork()==0){
		char *argv[]={"./p4",NULL};
		execvp(argv[0],argv);
	}
	//cout<<"p4 executed successfully\n";
	
	//for chat server
	int sfd = open("ffifo",O_RDONLY|O_NONBLOCK);
	//cout<<"Chat server started successfully\n";
	while(1){
		struct pollfd pfds[5];
		initPollFd(&pfds[0],STDIN_FILENO,POLLIN);
		initPollFd(&pfds[1],pp[0],POLLIN);
		initPollFd(&pfds[2],ffd,POLLIN);
		initPollFd(&pfds[3],fd,POLLIN);
		initPollFd(&pfds[4],sfd,POLLIN);
		//struct pollfd pfds[1];
		//initPollFd(&pfds[0],sfd,POLLIN);
		
		poll(pfds,5,-1);
		//polling inputs
		if(pfds[0].revents & POLLIN){
			char buff[MAX];
			scanf("%[^\n]%*c",buff);
			sendData(buff);
		}
		for(int i=1;i<4;i++){
			if(pfds[i].revents & POLLIN){
				//cout<<"POLLIN success for "<<i<<"\n";
				//fflush(stdout);
				char buff[MAX];
				read(pfds[i].fd,buff,MAX);
				sendData(buff);
			}
		}
		//Chat Server with clients
		if(pfds[4].revents & POLLIN){
			char msg[500];
			int cpid=0,j=0;
			char c;
			while(read(sfd,&c,1))
				msg[j++]=c;
			msg[j]='\0';
			j=0;
			while(isdigit(msg[j]))
				cpid=10*cpid+(msg[j++]-'0');
			//cout<<cpid<<" sent the message: "<<msg<<"\n";
			if(msg[strlen(msg)-1]=='|'){
				strncpy(wcfd[cnt],msg,strlen(msg)-1);	
				pids[cnt]=cpid;
				active[cnt]=1;
				cnt++;
				cout<<"Entry for client "<<wcfd[cnt-1]<<" created\n";
				cout<<cnt<<" total clients\n";
				fflush(stdout);
			}else{
				int sendingclient=search(cpid);
				if(active[sendingclient]==0)
					continue;
				//cout<<"entry found at "<<sendingclient<<"\n";
				for(int i=0;i<cnt;i++){
					if(i==sendingclient || active[i]==0)
						continue;
					int wfd=open(wcfd[i],O_WRONLY);
					write(wfd,&msg,strlen(msg));
					close(wfd);
				}
			}
		}
	}	
	return 0;
}
