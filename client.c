#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX 80
#define PORT 6506
#define SA struct sockaddr
#define SIZE 1024

void write_file(int sockfd, char *filename)
{
    int n;
    FILE *fp;
    char buffer[SIZE];

    fp = fopen(filename, "wb");
    if(fp==NULL)
    {
        perror("[-]Error in creating file.");
        exit(1);
    }
    while(1)
    {
        recv(sockfd, buffer, SIZE, 0);
        if(strncmp("end", buffer, 3) == 0) break;
        fprintf(fp, "%s", buffer);


    }
    fclose(fp);
    return;
}

void func(int sockfd)
{
    char buff[MAX];
    char filename[MAX];
    int n;
    for (;;) {
        bzero(buff, sizeof(buff));
        printf("Enter the string : ");
        n = 0;
        while ((buff[n++] = getchar()) != '\n')
            ;
        write(sockfd, buff, sizeof(buff));
        if ((strncmp(buff, "exit", 4)) == 0) {
            printf("Client Exit...\n");
            break;
        }
        if(strncmp("get", buff, 3) == 0){
            strncpy(filename, buff + 4, MAX - 4);
            filename[strcspn(filename, "\n")] = 0;
            bzero(buff, sizeof(buff));
            write_file(sockfd, filename);

        }
        else
        {
            read(sockfd, buff, sizeof(buff));
            printf("From Server : %s", buff);
        }
    }
}

int main()
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    // function for chat
    func(sockfd);

}
