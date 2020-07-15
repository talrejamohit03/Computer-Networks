#include "myHead.h"
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip.h>

int port = 51720;

int main() {
    int rsfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if(rsfd < 0) {
        perror("rsfd");
        exit(1);
    }

    int one = 1;
    if(setsockopt(rsfd, IPPROTO_IP, IP_HDRINCL, &one, sizeof(one)) < 0) {
        perror("setsockopt");
        exit(1);
    }

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    char packet[8192], *payload;

    struct iphdr *iph = (struct iphdr *)packet;
    struct tcphdr *tcph = (struct tcphdr *)(packet + sizeof(struct iphdr));
    //struct udphdr *udph = (struct udphdr *)(packet + sizeof(struct iphdr));

    memset(packet, 0, 8192);
    payload = packet + sizeof(struct iphdr) + sizeof(struct tcphdr);
    //payload = packet + sizeof(struct iphdr) + sizeof(struct udphdr);
    strcpy(payload, "testing");

    //fill ip header
    iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0;
	iph->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr) + strlen(payload);
	iph->id = htonl(54321);
	iph->frag_off = 0;
	iph->ttl = 255;
	iph->protocol = IPPROTO_TCP;
	iph->check = 0;
	iph->saddr = inet_addr("1.2.3.4");
	iph->daddr = saddr.sin_addr.s_addr;

    //ip checksum
    iph->check = chksum((unsigned short *) packet, iph->tot_len);

    //fill tcp header
    tcph->source = htons(51721);
	tcph->dest = htons(port);
	tcph->seq = 0;
	tcph->ack_seq = 0;
	tcph->doff = 0;
	tcph->fin = 0;
	tcph->syn = 1;
	tcph->rst = 0;
	tcph->psh = 0;
	tcph->ack = 0;
	tcph->urg = 0;
	tcph->window = htonl(65535);
	tcph->check = 0;
	tcph->urg_ptr = 0;

    while (1){
		if(sendto(rsfd, packet, iph->tot_len, 0,(struct sockaddr *)&saddr, sizeof(saddr)) < 0){
			perror("sendto failed");
		}
		else{
			printf("Packet Sent. Length : %d bytes\n", iph->tot_len);
		}
        sleep(1);
	}
}
