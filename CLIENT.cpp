#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<netdb.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>

#DEFINE PORT 2000

int main(int argc, char* argv[])
{
    int sockfd_client;
    sockaddr_in myaddr
    socklen_t len;

    if(argc!=2)
    {
        printf("error");
        exit(1);
    }

    if((sockfd_client=socket(AF_INET, SOCK_STREAM, 0)==-1))
    {
        perror("socket");
        exit(1);
    }

    myaddr.sin_family= AF_INET;
    myaddr.sin_port= htons(PORT);
    memset(&(myaddr.sin_zero), '/0', 8);

    if(inet_pton(AF_INET, argv[1], &myaddr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        exit(1);
    }

    if(connect(sockfd_client, (struct sockaddr *)&myaddr, len)==-1)
    {
        perror("connect");
        exit(1);
    }



}
