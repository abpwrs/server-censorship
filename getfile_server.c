/* simple server to request and redact a requested file on behalf of a client */
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
#define BASE_URL "http://user.engineering.uiowa.edu/~jwryan/Communication_Networks/" // base path to files
// args
// method -- 0
// word_to_replace -- 1
// server_port -- 2
int main(int argc, char *argv[])
{
    int sockfd, client_sockfd;
    char *word_to_replace = argv[1];
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
        printf("Connected to client at 127.0.0.1:%d\n", port);

        // read in file name and curl the file if possible
        nread = read(client_sockfd, buf, SIZE);
        printf("Downloading: %s...\n", buf);
        char *requested_file = (char *)malloc(sizeof(char) * SIZE);
        sprintf(requested_file, "%s", buf);
        char *command = (char *)malloc(sizeof(char) * SIZE);
        sprintf(command, "%s %s%s %s", "curl", BASE_URL, requested_file, "-s -o download.txt --fail");
        int status = system(command);
        free(requested_file);
        free(command);

        // write status back to user
        sprintf(buf, "%d", status);
        len = strlen(buf) + 1;
        write(client_sockfd, buf, len);

        // if the request failed
        if (status != 0)
        {
            // failure to download file
            printf("Bad request\nClosing connection\n");
            close(client_sockfd);
            continue;
        }
        else
        {
            // success in downloading -- censor and transmit
            printf("Download success\n");
            FILE *f = fopen("download.txt", "r");
            // This code to find the file size came from stackoverflow
            fseek(f, 0, SEEK_END);
            long fsize = ftell(f);
            fseek(f, 0, SEEK_SET); //same as rewind(f);

            char *string = malloc(fsize + 1);
            fread(string, fsize, 1, f);
            char *found = strstr(string, word_to_replace);
            fclose(f);
            int num_replaced = 0;
            while (found)
            {
                for (int offset = 0; offset < strlen(word_to_replace); ++offset)
                {
                    *(found + offset) = '*';
                }
                ++num_replaced;
                found = strstr(string, word_to_replace);
            }
            printf("Replacement count: %d\n", num_replaced);

            // string has been censored and is ready to transmion
            for (int i = 0; i < fsize; i += SIZE)
            {
                char sub[SIZE]; 
                for(int j = 0; j < SIZE; ++j){
                    sub[j] = string[i+j];
                }
                write(client_sockfd, sub, SIZE);
            }
            write(client_sockfd, "", 0);
            free(string);
        }
        printf("Closing connection\n");
        close(client_sockfd);
    }
}
