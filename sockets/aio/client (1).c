#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <poll.h>

#define h_addr h_addr_list[0]

struct msg {
    char u_name[100]; //username
    char data[256]; //message content
};

//portno should be known prior
int sfd, portno = 51720;
char *name = "localhost"; //name can be ipv4 address(leave it as localhost if local chatting)
struct sockaddr_in serv_addr, cli_addr;//server_details address
struct hostent *server_details;//server_details details
struct msg buff, buff1;

int myConnect() {
    if(connect(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        return -1;
    }
    
    int clilen = sizeof(cli_addr);
    getsockname(sfd, (struct sockaddr *)&cli_addr, &clilen);
    char buff1[100], temp[100];
    sprintf(temp, "%d", cli_addr.sin_addr.s_addr);
    inet_ntop(AF_INET, temp, buff1, sizeof(buff1));
    printf("client %s::%d\n", buff1, ntohs(cli_addr.sin_port));
    
    return 0;
}

int main() {

    //gets server details by name of the server
    server_details = gethostbyname(name);
    if(server_details == NULL) {
        perror("Invalid host name");
        exit(1);
    }

    if((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    //initialize serv_addr
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);//htons adjusts byte ordering

    //copy server address from server_details
    bcopy((char *) server_details->h_addr, 
         (char *) &serv_addr.sin_addr.s_addr, 
         server_details->h_length);

    int f = myConnect();
    if(f == -1) {
        printf("Could not connect terminating...\n");
        exit(1);
    }

    printf("Enter username :");
    fgets(buff.u_name, 99, stdin);
    buff.u_name[strlen(buff.u_name) - 1] = '\0';

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(0, &readfds);
    FD_SET(sfd, &readfds);
    int maxfd = sfd;
    while(1) {
        select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if(FD_ISSET(0, &readfds)) {
            fgets(buff.data, 255, stdin);
            buff.data[strlen(buff.data) - 1] = '\0';
            send(sfd, &buff, sizeof(buff), 0);
        }
        if(FD_ISSET(sfd, &readfds)) {
            recv(sfd, &buff1, sizeof(buff1), 0);
            if(strlen(buff1.data) == 0) {
                exit(1);
            }
            if(strlen(buff1.u_name) == 0) {
                printf("%s\n", buff1.data);
            }
            else {
                printf("%s : %s\n", buff1.u_name, buff1.data);
            }
        }
        FD_SET(0, &readfds);
        FD_SET(sfd, &readfds);
    }
}
