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

int sfd, portno = 37774,pid;
char *name = "localhost"; //name can be ipv4 address(leave it as localhost if local chatting)
struct sockaddr_in serv_addr;//server_details address
struct hostent *server_details;//server_details details
char pro[20];

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
    serv_addr.sin_port = htons(portno);//htons adjusts byte ordering

    //copy server address from server_details
    bcopy((char *) server_details->h_addr, 
         (char *) &serv_addr.sin_addr.s_addr, 
         server_details->h_length);
	
	if(connect(sfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting to proxy");
        return 1;
    }
    int firstmsg = 1;
    while(1) {
        struct pollfd fds[2];

        fds[0].fd = 0;
        fds[0].events = POLLIN;

        fds[1].fd = sfd;
        fds[1].events = POLLIN;

        poll(fds, 2, -1);

        //input from keyboard
        if(fds[0].revents & POLLIN){
			char buff[500],inp[500];
			char c;
			int j=0;
			read(STDIN_FILENO,&c,1);
			while(c!='\n'){
				inp[j++]=c;
				read(STDIN_FILENO,&c,1);
			}
			inp[j]='\0';
			//cout<<inp<<" is the input\n";
			buff[0]='\0';
			strcat(buff,pro);
			strcat(buff,": ");
			strcat(buff,inp);
			
			write(sfd,buff,strlen(buff)+1);
			printf("%s written to proxy server\n",buff);
		}
        if(fds[1].revents & POLLIN) {
        	//printf("POLLIN encountered\n");
        	char msg[MAX];
			recv(sfd,msg,MAX,0);
            printf("%s\n", msg);
            if(firstmsg == 1){
            	firstmsg = 0;
            	int sno;
            	scanf("%d",&sno);
            	send(sfd,&sno,sizeof(sno),0);
            	printf("%d written service number\n",sno);
            }
        }
    }
}
