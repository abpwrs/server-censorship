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
#define TIME_PORT 16200                                                              //server will listen on this port
#define BASE_URL "http://user.engineering.uiowa.edu/~jwryan/Communication_Networks/" // base path to files
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

    // Get the port from CLI
    int port = atoi(argv[2]);
    printf("Listening on port: %d\n", port);
    serv_addr.sin_port = htons(port);
    
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
        printf("Waiting for connection...\n");
        client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &len);
        if (client_sockfd == -1)
        {
            perror(NULL);
            continue;
        }
        printf("Connected to client at 127.0.0.1:%d",TIME_PORT);

        // read in file name
        nread = read(client_sockfd, buf, SIZE);
        printf("file requested: %s\n", buf);
        char *requested_file = buf;

        char *command = (char *)malloc(sizeof(char) * SIZE);
        sprintf(command, "%s %s%s %s %s", "curl", BASE_URL, requested_file, "-o", requested_file);
        printf("%s\n", command);
        int status = system(command);
        free(command);
        // return status of file download
        printf(buf, "%d\n", status);
        len = strlen(buf) + 1;
        write(client_sockfd, buf, len);

        /* transfer data */
        //time(&t);
        sprintf(buf, "%s -- %d\n", "This should be a file contents", ++num_connections);
        len = strlen(buf) + 1;
        write(client_sockfd, buf, len);
        close(client_sockfd);
    }
}