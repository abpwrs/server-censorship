/* TCP client that requests a file and recieves a redacted file*/
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

    // Get the port from CLI
    int port = atoi(argv[3]);
    printf("Connecting to port 127.0.0.1:%d\n", port);
    serv_addr.sin_port = htons(port);

    int len = sizeof(serv_addr);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror(NULL);
        exit(3);
    }
    /* transfer data */

    // Send File Name
    printf("Requested: %s\n", argv[1]);
    sprintf(buf, "%s", argv[1]);
    len = strlen(buf) + 1;
    write(sockfd, buf, len);

    // recieve status
    nread = read(sockfd, buf, SIZE);
    if (atoi(buf) != 0)
    {
        // failed to download file, close connection and exit code 0
        printf("Bad request\nClosing connection\n");
        close(sockfd);
        exit(0);
    }
    // read in the censored file
    printf("Recieving file...\n");
    FILE *f = fopen(argv[1], "w");
    do
    {
        nread = read(sockfd, buf, SIZE);
        fputs(buf,f);
    } while (nread != 0 );
    fclose(f);
    printf("Transfer Complete\n");
    close(sockfd);
    exit(0);
}