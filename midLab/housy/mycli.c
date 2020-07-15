#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <poll.h>
#include <signal.h>
#include <fcntl.h>

#define h_addr h_addr_list[0]
#define MAX 100
int sfd, portno1 = 37777, portno2 = 37778,pid;
char *name = "localhost"; //name can be ipv4 address(leave it as localhost if local chatting)
struct sockaddr_in serv_addr;//server_details address
struct hostent *server_details;//server_details details
char pro[20];

int coup[5];
int main(){
	pid=getpid();
	sprintf(pro,"%d",pid);
	printf("%d client started\n",pid);
	server_details = gethostbyname(name);
    if(server_details == NULL) {
        perror("Invalid host name");
        return 1;
    }

    if((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return 1;
    }
    
    //initialize serv_addr
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno1);//htons adjusts byte ordering
    
    
    //copy server address from server_details
    bcopy((char *) server_details->h_addr, 
         (char *) &serv_addr.sin_addr.s_addr, 
         server_details->h_length);
	
	if(connect(sfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting");
        return 1;
    }
    recv(sfd,coup,5*sizeof(int),0);
    printf("Coupon received: ");
    for(int i=0;i<5;i++)
    	printf("%d ",coup[i]);
    printf("\n");
    int matched = 0;
    while(matched<5){
    	int num;
    	recv(sfd,&num,sizeof(int),0);
    	printf("%d received\n",num);
    	for(int i=0;i<5;i++){
    		if(num == coup[i]){
    			matched++;
    		}
    	}
    }
    int sfd2;
    if((sfd2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return 1;
    }
    serv_addr.sin_port = htons(portno2);//htons adjusts byte ordering
    if(connect(sfd2, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting");
        return 1;
    }
    send(sfd2,coup,sizeof(coup),0);
    char buff[100];
    recv(sfd2,buff,100,0);
    printf("%s\n",buff);
    printf("Client Ended\n");
}
