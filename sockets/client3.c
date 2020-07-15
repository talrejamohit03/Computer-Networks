#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

int main()
{
 
 struct sockaddr_in serveraddr,clientaddr;
 struct hostent *host;
 socklen_t addr_len;
 int sfd;
 sfd=socket(AF_INET,SOCK_STREAM,0);
 if(sfd==-1)
 {
   printf("error creating socket\n");
   exit(0);
 }
 
 host=gethostbyname("127.0.0.1");
 
 serveraddr.sin_family=AF_INET;
 serveraddr.sin_port=htons(8080);
 serveraddr.sin_addr=*(struct in_addr*)host->h_addr;
 
 bzero((void *)(&(serveraddr.sin_zero)),8);

  addr_len=sizeof(serveraddr);
 int x=connect(sfd,(struct sockaddr*)&serveraddr,addr_len);
 if(x==-1)
 {
    printf("error in connecting\n");
    exit(0);
 }
 
 while(1)
 {
  char recv_buff[1024];
  char send_buff[1024];
  
  printf("press q to exit or enter data\n");
  fgets(send_buff,1024,stdin);
 // printf("%d\n",strlen(send_buff));
  if(send_buff[0]=='q')
  {
    //printf("flag3\n");
    close(sfd);
    exit(0);
  }
  
   int y=send(sfd,&send_buff,strlen(send_buff),0);
   if(y<0)
   {
      printf("error in send\n");
      exit(0);
   }
   //printf("reached here\n");
    int n=recv(sfd,&recv_buff,1024,0);
    
    if(recv_buff[0]=='q')
    {
     //printf("flag4\n");
     close(sfd);
     exit(0);
    }
     recv_buff[n]='\0';
     
     printf("Message received is :%s\n",recv_buff);
     
    //memset(recv_buff,0,sizeof(recv_buff));
    //memset(send_buff,0,sizeof(send_buff));
 }
 
}
