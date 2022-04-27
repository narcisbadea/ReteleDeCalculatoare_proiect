#include <stdio.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>



#define MAX 80
#define PORT 6506
#define SA struct sockaddr
#define SIZE 1024

void send_file(FILE *fp, int sockfd)
{
    char data[SIZE] = {0};
    while(fgets(data, SIZE, fp)!=NULL)
    {

        if(send(sockfd, data, sizeof(data), 0)== -1)
        {
            perror("[-] Error in sending data");
            exit(1);
        }
        bzero(data, SIZE);
    }
    send(sockfd, "end", sizeof("end"), 0);
}

char* _ls(char *caleSursa)
{
    DIR *dr;
    struct stat stare;
    struct dirent *entryData;
    char* fisiere=malloc(2000);

    stat(caleSursa, &stare);

    if (S_ISDIR(stare.st_mode))
    {
        dr = opendir(caleSursa);
        while ((entryData = readdir(dr)) != NULL)
        {
            if(strcmp(entryData->d_name,"..") != 0 && strcmp(entryData->d_name, ".") != 0 ){
                strcat(fisiere,entryData->d_name);
                strcat(fisiere,"\t");
            }
        }
        strcat(fisiere,"\n");
        printf("\n");
    }
    return fisiere;
}

// Function designed for chat between client and server.
void func(int connfd)
{
    char buff[MAX];
    char filename[20];
    char sursa[50];
    int n;
    FILE *fp;
    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX);


        // read the message from client and copy it in buffer
        read(connfd, buff, sizeof(buff));

        printf("From client: %s", buff);

        if(strncmp("ls", buff, 2) == 0){
            bzero(buff, MAX);
            strcpy(buff,_ls("sursa"));
            write(connfd, buff, sizeof(buff));
            bzero(buff, MAX);
        }else if(strncmp("get", buff, 3) == 0){
            strncpy(filename, buff + 4, MAX - 4);
            strcpy(sursa, "sursa/");
            strcat(sursa, filename);
            sursa[strcspn(sursa, "\n")] = 0;
            fp = fopen(sursa, "rb");
            fseek(fp, 0, SEEK_END);
            fseek(fp, 0, SEEK_SET);

            bzero(buff, MAX);
            if(fp == NULL)
            {
                perror("[-]Error in reading file.");
                exit(1);
            }
            send_file(fp, connfd);
            strcpy(buff,"[+] File data send successfully. \r\n\r\n");
            bzero(buff, MAX);
        }




        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }
}

// Driver function
int main()
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;



    while(1) {


        // socket create and verification
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            printf("socket creation failed...\n");
            exit(0);
        } else
            printf("Socket successfully created..\n");
        bzero(&servaddr, sizeof(servaddr));

        // assign IP, PORT
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(PORT);

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
            printf("setsockopt(SO_REUSEADDR) failed");

        // Binding newly created socket to given IP and verification
        if ((bind(sockfd, (SA *) &servaddr, sizeof(servaddr))) != 0) {
            printf("socket bind failed...\n");
            exit(0);
        } else
            printf("Socket successfully binded..\n");
        // Now server is ready to listen and verification
        if ((listen(sockfd, 5)) != 0) {
            printf("Listen failed...\n");
            exit(0);
        } else
            printf("Server listening..\n");
        len = sizeof(cli);

        // Accept the data packet from client and verification
        connfd = accept(sockfd, (SA *) &cli, &len);
        if (connfd < 0) {
            printf("server accept failed...\n");
            exit(0);
        } else
            printf("server accept the client...\n");

        // Function for chatting between client and server
        func(connfd);

        // After chatting close the socket
        close(sockfd);
    }
}