#include "myHead.h"
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/udp.h>

#define h_addr h_addr_list[0]

int main() {

    int cli_port = 51720;
    int sfd;
    char *name = "localhost";
    struct sockaddr_in serv_addr, cli_addr;
    struct hostent *server_details;
    server_details = gethostbyname(name);
    
    memset(&cli_addr, 0, sizeof(cli_addr));
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_port = htons(cli_port);
    bcopy((char *) server_details->h_addr, 
         (char *) &cli_addr.sin_addr.s_addr, 
         server_details->h_length);

    //create socket
    if((sfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0) {
        perror("socket");
        exit(1);
    }

    //setsockopt to reuse the same address
    int opt = 1;
    if((setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))) < 0) {
        perror("setsockopt");
        exit(1);
    }

    //bind address to socket fd
    if(bind(sfd, (struct sockaddr*)&cli_addr, sizeof(cli_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    while(1) {
        char buff[100];
        int len = sizeof(serv_addr);
        int r = recvfrom(sfd, buff, 100, 0, (struct sockaddr *)&serv_addr, &len);
        //extract ip header
        struct iphdr *iph = (struct iphdr*)buff;

        //convert ip address from number to . format
        char buff1[100];
        struct in_addr in;
        in.s_addr = iph->saddr;
        strcpy(buff1, inet_ntoa(in));
        
        printf("%s from : %s\n", buff + sizeof(struct iphdr) + sizeof(struct tcphdr), buff1);
    }
}
