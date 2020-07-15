#include"header.h"

int main(){
	int sfd;
	struct sockaddr_in serv_addr;
	int port_no=17736;

	sfd = socket(AF_INET,SOCK_STREAM,0);
	bzero(&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_no);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	int opt=1;
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt)); 
	bind(sfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr));
	listen(sfd,10);
	int nsfd;
	nsfd = accept(sfd , NULL,NULL);
	char buf[20];
	while(1){
		bzero(buf,20);
		recv(nsfd,buf,20,0);
		printf("Recv:%s\n",buf);
		fflush(stdout);
		strcat(buf,":R:");
		send(nsfd,buf,20,0);
	}
	return 0;
}
