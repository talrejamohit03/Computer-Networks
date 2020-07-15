#include"header.h"

struct msgbuf{
	long type;
	char msg[10];
};

int f=0;

void hdfn(){
	printf("SIG c2\n");
	f=1;
}
int main(){
	int usfd,sfd;
	struct sockaddr_un userv_addr;
  	int userv_len;
  	usfd = socket(AF_UNIX, SOCK_DGRAM, 0);
  	bzero(&userv_addr,sizeof(userv_addr));
  	userv_addr.sun_family = AF_UNIX;
   	strcpy(userv_addr.sun_path, "c2");
	userv_len = sizeof(userv_addr);
	unlink("c2");
	bind(usfd,(struct sockaddr*)&userv_addr,userv_len);
	
	
	key_t key=ftok("ESS",23);
	int msqid=msgget(key,0666|IPC_CREAT);	
	char buf[20];
	signal(SIGUSR1,hdfn);
	int c;
	while(1){
		printf("Enter 1 to add to msg_queue\n");
		scanf("%d",&c);
		if(c==1){
			struct msgbuf msg;
			msg.type=(long)getpid();
			strcpy(msg.msg,"c2");
			msgsnd(msqid,&msg,sizeof(msg)-sizeof(long),0);
			break;
		}
	}
	printf("start\n");
	while(1){
		if(f==1){
			sfd=recv_fd(usfd);
			bzero(buf,20);
			printf("Enter :");
			scanf("%s",buf);
			send(sfd,buf,strlen(buf)+1,0);
			bzero(buf,20);
			recv(sfd,buf,20,0);
			printf("Recv:%s\n",buf);
			struct msgbuf msgs,msgr;
			msgs.type=(long)getpid();
			strcpy(msgs.msg,"c2");
			msgsnd(msqid,&msgs,sizeof(msgs)-sizeof(long),0);
			msgrcv(msqid,&msgr,sizeof(msgr)-sizeof(long),0,0);
			struct sockaddr_un ucli_addr;
		  	int ucli_len;
		  	bzero(&ucli_addr,sizeof(ucli_addr));
		  	ucli_addr.sun_family = AF_UNIX;
		   	strcpy(ucli_addr.sun_path,msgr.msg);
		   	send_fd(usfd,sfd,(void *)&ucli_addr);
		   	f=0;
		   	int pid=msgr.type;
		   	kill(pid,SIGUSR1);
		   	//sleep(1);close(sfd);
		}
	}
	return 0;
}
