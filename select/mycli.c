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

int sfd, portno = 37777,pid;
char *name = "localhost"; //name can be ipv4 address(leave it as localhost if local chatting)
struct sockaddr_in serv_addr;//server_details address
struct hostent *server_details;//server_details details
char pro[20];

int max(int x, int y) {
	return (x>y)?x:y;  
} 

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
    //bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);//htons adjusts byte ordering
	inet_aton("127.0.0.1",&serv_addr.sin_addr.s_addr);
    //copy server address from server_details
    /*
    bcopy((char *) server_details->h_addr, 
         (char *) &serv_addr.sin_addr.s_addr, 
         server_details->h_length);
	*/
	if(connect(sfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting");
        return 1;
    }
    
    fd_set rset;
    //clearing the descriptor set
	FD_ZERO(&rset);
	
    while(1) {
    	//setting sfd in read set
		FD_SET(sfd,&rset);
		FD_SET(STDIN_FILENO,&rset);
		//get maxfd
		int maxfd = max(sfd,STDIN_FILENO) + 1;
		
		select(maxfd,&rset,NULL,NULL,NULL);//timeout is NULL it will wait forever

        //input from keyboard
        if(FD_ISSET(STDIN_FILENO,&rset)){
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
			
			write(sfd,&buff,strlen(buff)+1);
			//cout<<buff<<"written to ffifo\n";
		}

        //data from other clients
        if(FD_ISSET(sfd,&rset)) {
        	char msg[500];
			read(sfd,msg,100);
            printf("%s\n", msg);
        }
    }
}
