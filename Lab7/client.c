#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#define BUFF_SIZE 80


int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Wrong num of arguments!\n");
        printf("correct syntax: \"client <IP address> <Port number> \"\n");
        return 1;
    }

    int sockfd, rcvBytes, sendBytes;
    socklen_t len;
    char buff[BUFF_SIZE + 1];


    //char SERV_ADDR[50];
    struct sockaddr_in servaddr;
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
    {
        printf("[%s] is a invalid IP address\n", argv[1]);
        exit(1);
    }
    if (atoi(argv[2]) < 0 && atoi(argv[2]) > 65535)
    {
        printf("[%s] is a invalid Port number\n", argv[2]);
        exit(1);
    }

    //Step 1: Construct socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error: ");
        return 0;
    }

    // Step2: Specify server address
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	
	// Step3: Request to connect server
	if(connect(sockfd, (struct sockaddr*)&servaddr, sizeof(struct sockaddr)) < 0){
		printf("\nError! Can not connect to sever! ");
		return 0;
	}
		
	// Step4: Communicate with server
    while (1)
    {
        printf("---\nInput: ");
        fgets(buff, sizeof(buff), stdin);
        buff[strcspn(buff, "\n")] = '\0'; // replace \n with \0

        len = sizeof(servaddr);
        sendBytes = send(sockfd, buff, strlen(buff), 0);
        if (sendBytes < 0)
        {
            perror("Error: ");
            break;
        }
        if (strlen(buff) == 0)
        {
            break;
        }
        rcvBytes = recv(sockfd, buff, BUFF_SIZE, 0);
        if (rcvBytes < 0)
        {
            perror("Error: ");
            break;
        }
        buff[rcvBytes] = '\0';
        printf("Reply: %s\n\n", buff);


    }    
    close(sockfd);
    return 1;
}
