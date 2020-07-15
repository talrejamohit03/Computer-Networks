#include"header.h"

struct msgbuf{
	long type;
	char msg[10];
};

int f=1;

void hdfn(){
	printf("C1:SIGUSR1\n");
	f=1;
}

int main(){
	int sfd;
	struct sockaddr_in serv_addr;
	int port_no;
	printf("enter port no:");
	scanf("%d",&port_no);
	bzero(&serv_addr,sizeof(serv_addr));
	sfd = socket(AF_INET , SOCK_STREAM , 0);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_no);
	inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr);
	connect(sfd , (struct sockaddr *)&serv_addr , sizeof(serv_addr));
	

	int usfd;
	struct sockaddr_un userv_addr;
  	int userv_len;
  	usfd = socket(AF_UNIX, SOCK_DGRAM, 0);
  	bzero(&userv_addr,sizeof(userv_addr));
  	userv_addr.sun_family = AF_UNIX;
   	strcpy(userv_addr.sun_path, "c1");
	userv_len = sizeof(userv_addr);
	unlink("c1");
	bind(usfd,(struct sockaddr*)&userv_addr,userv_len);
	
	
	key_t key=ftok("ESS",23);
	int msqid=msgget(key,0666|IPC_CREAT);	
	char buf[20];
	signal(SIGUSR1,hdfn);
	int first=1;
	while(1){
		if(f==1){
			if(first!=1){
				sfd=recv_fd(usfd);
				first=0;
			}
			bzero(buf,20);
			printf("Enter :");
			scanf("%s",buf);
			send(sfd,buf,strlen(buf)+1,0);
			bzero(buf,20);
			recv(sfd,buf,20,0);
			printf("Recv:%s\n",buf);
			struct msgbuf msgs,msgr;
			msgs.type=(long)getpid();
			strcpy(msgs.msg,"c1");
			msgsnd(msqid,&msgs,sizeof(msgs)-sizeof(long),0);
			//printf("Msg sent to queue\n");
			msgrcv(msqid,&msgr,sizeof(msgr)-sizeof(long),0,0);
			//printf("msg rcv %s\n",msgr.msg);
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
