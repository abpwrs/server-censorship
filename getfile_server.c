// simple server to request and redact file d
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#define SIZE 1024
char buf[SIZE];
#define TIME_PORT 16200 //server will listen on this port
// args
// method -- 0
// word_to_replace -- 1
// server_port -- 2
int main(int argc, char *argv[])
{
    int sockfd, client_sockfd;
    int nread, len;
    struct sockaddr_in serv_addr, client_addr;
    time_t t;
    int num_connections = 0;
    if (argc != 3)
    { // run input validation and prompt for the correct num of args
        printf("Incorrect usage of %s\n", argv[0]);
        printf("Usage: %s word_to_replace server_port\n", argv[0]);
        exit(2);
    }
    /* create endpoint */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror(NULL);
        exit(2);
    }
    /* bind address */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(TIME_PORT);
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror(NULL);
        exit(3);
    }
    /* specify queue */
    len = sizeof(client_addr);
    listen(sockfd, 5);
    for (;;)
    {
        client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &len);
        if (client_sockfd == -1)
        {
            perror(NULL);
            continue;
        }

        nread = read(client_sockfd, buf, SIZE);
        printf("file requested: %s\n", buf);
        /* transfer data */
        //time(&t);
        sprintf(buf, "%s -- %d\n", "This should be a file contents", ++num_connections);

        len = strlen(buf) + 1;
        write(client_sockfd, buf, len);
        close(client_sockfd);

    }
}