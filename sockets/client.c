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

struct msg {
    char u_name[100];
    char data[256];
};

//portno should be known prior
int sfd, portno = 51720;
char *name = "localhost"; //name can be ipv4 address(leave it as localhost if local chatting)
struct sockaddr_in serv_addr;//server_details address
struct hostent *server_details;//server_details details

//handler function to close opened fds when signalled
void hdfn(int signo) {
    close(sfd);
    signal(SIGINT, SIG_DFL);
    raise(SIGINT);
}

int myConnect() {
    if(connect(sfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting");
        exit(1);
    }
    struct msg buff;
    read(sfd, &buff, sizeof(buff));
    if(!strcmp(buff.data, "connected")) {
        return 0;
    }
    return -1;
}

int main() {
    signal(SIGINT, hdfn);

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

    //buff for other clients buff1 for keyboard
    struct msg buff, buff1;
    printf("Enter a user name: ");
    fgets(buff1.u_name, 255, stdin);
    buff1.u_name[strlen(buff1.u_name) - 1] = '\0';

    while(1) {
        struct pollfd fds[2];

        fds[0].fd = 0;
        fds[0].events = POLLIN;

        fds[1].fd = sfd;
        fds[1].events = POLLIN;

        int p = poll(fds, 2, -1);

        //input from keyboard
        if(fds[0].revents & POLLIN) {
            fgets(buff1.data, 255, stdin);
            buff1.data[strlen(buff1.data) - 1] = '\0';
            write(sfd, &buff1, sizeof(buff1));
        }

        //data from other clients
        if(fds[1].revents & POLLIN) {
            read(sfd, &buff, sizeof(buff));
            printf("%s : %s\n", buff.u_name, buff.data);
        }
    }
}
