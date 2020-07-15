#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<signal.h>

//int sfd=0;
/*
void hdfn(int sig)
{
    close(sfd);
    exit(0);
}*/

int main()
{
 //signal(SIGINT,hdfn);
 struct sockaddr_in serveraddr,clientaddr;
 int sfd;
 sfd=socket(AF_INET,SOCK_STREAM,0);
 if(sfd==-1)
 {
   printf("error creating socket\n");
   exit(1);
 }

 serveraddr.sin_family=AF_INET;
 serveraddr.sin_port=htons(8080);
 serveraddr.sin_addr.s_addr=INADDR_ANY;
  bzero((void *)(&(serveraddr.sin_zero)),8);
  
 socklen_t addr_len=sizeof(serveraddr);
 
 //bzero(&serveraddr,addr_len);
 
 int x=bind(sfd,(struct sockaddr*)&serveraddr,addr_len);
 if(x==-1)
 {
    printf("error in binding address\n");
    exit(1);
 }
 
 int y=listen(sfd,1024);
 if(y==-1)
 {
   printf("error in listening\n");
   exit(1);
 }
  //printf("idhar tak\n");
  
 while(1)
 { 
  addr_len=sizeof(struct sockaddr_in);
   int nsfd=accept(sfd,(struct sockaddr*)&clientaddr,&addr_len);
   printf("%d\n",nsfd);
   if(nsfd<=0)
   {
    printf("error in accepting client\n");
    exit(1);
   }
   //printf("yaha tak\n");
   
   
   int c=fork();
   
   if(c==0)
   {
      close(sfd);
     while(1)
     {
     char recv_buff[1024];
     char send_buff[1024];
     int n=recv(nsfd,&recv_buff,1024,0);
    
     
     if(recv_buff[0]=='q')
     {
      //printf("flag1\n");
      close(nsfd);
      exit(0);
     }
      recv_buff[n]='\0';
     printf("Message received is :%s\n",recv_buff);
     printf("press q to exit or enter data \n");
     
     fgets(send_buff,1024,stdin);
     
     if(send_buff[0]=='q')
     {
      //printf("flag2\n");
      close(nsfd);
      exit(0);
     }
     
     int z=send(nsfd,&send_buff,strlen(send_buff),0);
     if(z<0)
     {
      printf("error in send\n");
      exit(1);
     }
     
     //memset(recv_buff,0,sizeof(recv_buff));
     //memset(send_buff,0,sizeof(send_buff));
    }
 
   }
   else
   {
      close(nsfd);
   }
   
 }
 
}
