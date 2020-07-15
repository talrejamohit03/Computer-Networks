#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>
#include <poll.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/msg.h>

#define h_addr h_addr_list[0]

struct msg {
    int type;
    char buff[1];
};

//portno should be known prior
int sfd, portno = 51720, msgqid;
char *name = "localhost"; //name can be ipv4 address(leave it as localhost if local chatting)
struct sockaddr_in serv_addr, cli_addr;//server_details address
struct hostent *server_details;//server_details details

//handler function to close opened fds when signalled
void hdfn(int signo) {
    close(sfd);
    msgctl(msgqid, IPC_RMID, NULL);
    unlink("ffo1");
    unlink("ffo2");    
    signal(SIGINT, SIG_DFL);
    raise(SIGINT);
}

void hdfn1(int signo) {
    char buff[256];
    for(int i = 0; i < 2; i++) {
        fgets(buff, 255, stdin);
        buff[(int)strlen(buff) - 1] = '\0';
        send(sfd, buff, strlen(buff) + 1, 0);
        recv(sfd, buff, 100, 0);
        printf("%s\n", buff);
    }
    struct msg m;
    msgrcv(msgqid, &m, sizeof(m), 0, 0);
    printf("Giving turn to : %d\n", m.type);
    kill(m.type, SIGUSR1);
    m.type = getpid();
    msgsnd(msgqid, &m, sizeof(m), 0);
}

int myConnect() {
    if(connect(sfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error connecting");
        return -1;
    }
    
    int clilen = sizeof(cli_addr);
    getsockname(sfd, (struct sockaddr *)&cli_addr, &clilen);
    char buff1[100], temp[100];
    sprintf(temp, "%d", cli_addr.sin_addr.s_addr);
    inet_ntop(AF_INET, temp, buff1, sizeof(buff1));
    printf("client %s::%d\n", buff1, ntohs(cli_addr.sin_port));

    return 1;
}

int main() {
    printf("pid : %d\n", getpid());
    signal(SIGINT, hdfn);
    signal(SIGUSR1, hdfn1);

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

    key_t k = ftok("msgq", 65);
    msgqid = msgget(k, 0666 | IPC_CREAT);
    mkfifo("ffo1", 0666);
    mkfifo("ffo2", 0666);
    int rfd, wfd;
    raise(SIGUSR1);
    char buff[256];
    while(1) {
        rfd = open("ffo1", O_RDONLY);
        read(rfd, buff, 255);
        close(rfd);
        send(sfd, buff, strlen(buff) + 1, 0);
        recv(sfd, buff, strlen(buff) + 1, 0);
        wfd = open("ffo2", O_WRONLY);
        write(wfd, buff, strlen(buff) + 1);
        close(wfd);
    }
}
