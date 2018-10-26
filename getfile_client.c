/* TCP client that finds the time from a server */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#define SIZE 1024
char buf[SIZE];
#define TIME_PORT 16200 //server listens on this port
// args:
// method -- 0
// filename -- 1
// host -- 2
// port -- 3
int main(int argc, char *argv[])
{
    int sockfd;
    int nread;
    struct sockaddr_in serv_addr;
    struct hostent *h;
    if (argc != 4)
    {
        printf("Incorrect usage of %s\n", argv[0]);
        fprintf(stderr, "usage: %s filename host port\n", argv[0]);
        exit(1);
    }
    /* create endpoint */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror(NULL);
        exit(2);
    }
    /* connect to server */
    serv_addr.sin_family = AF_INET;
    h = gethostbyname(argv[2]);
    bcopy(h->h_addr, (char *)&serv_addr.sin_addr, h->h_length);
    serv_addr.sin_port = htons(TIME_PORT);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror(NULL);
        exit(3);
    }
    /* transfer data */
    nread = read(sockfd, buf, SIZE);
    write(1, buf, nread); // Writes to standard output
    close(sockfd);
    exit(0);
}