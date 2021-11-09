#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#define BUFF_SIZE 80
#define SERV_PORT 8080

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
    char buff[BUFF_SIZE + 1], name[30], newpass[30];
    int count = 0;

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
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Error: ");
        return 0;
    }

    //Step 2: Define the address of the server
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    //servaddr.sin_addr = inet_aton(SERV_ADDR, &servaddr.sin_addr);
    servaddr.sin_port = htons(atoi(argv[2]));

    //Step 3: Communicate with server
    while (1)
    {
        printf("Send to server: ");
        fgets(buff, sizeof(buff), stdin);
        buff[strcspn(buff, "\n")] = '\0'; // replace \n with \0
        if (strlen(buff) == 0)
        {
            return 0;
        }
        if (count == 0)
        {
            strcpy(name, buff);
            count++;
        }
        len = sizeof(servaddr);
        sendBytes = sendto(sockfd, buff, strlen(buff), 0,
                           (struct sockaddr *)&servaddr, len);
        if (sendBytes < 0)
        {
            perror("Error: ");
            return 0;
        }
        rcvBytes = recvfrom(sockfd, buff, BUFF_SIZE, 0,
                            (struct sockaddr *)&servaddr, &len);
        if (rcvBytes < 0)
        {
            perror("Error: ");
            return 0;
        }
        buff[rcvBytes] = '\0';
        printf("Reply from server: %s\n\n", buff);

        if (strcmp(buff, "OK") == 0)
        {
            if (count == 0)
            {
                strcpy(name, buff);
                count++;
            }
            while (1)
            {
                printf("Send to server: ");
                fgets(newpass, sizeof(newpass), stdin);
                newpass[strcspn(newpass, "\n")] = '\0';
                if (strlen(newpass) == 0)
                {
                    return 0;
                }
                sendBytes = sendto(sockfd, newpass, strlen(newpass), 0,
                                   (struct sockaddr *)&servaddr, len);
                if (sendBytes < 0)
                {
                    perror("Error");
                    break;
                }
                char number_pass[10], str_pass[10];
                rcvBytes = recvfrom(sockfd, number_pass, sizeof(number_pass), 0,
                                    (struct sockaddr *)&servaddr, &len);
                if (rcvBytes < 0)
                {
                    perror("Error");
                    break;
                }
                number_pass[rcvBytes] = '\0';
                if (strcmp(number_pass, "Goodbye") == 0)
                {
                    printf("%s %s\n\n", number_pass, name);
                    count = 0;
                    break;
                }
                else if (strcmp(number_pass, "Error") == 0)
                {
                    printf("Reply from server: %s\n\n", number_pass);
                }
                else
                {
                    printf("Reply from server: \n%s\n", number_pass);
                    rcvBytes = recvfrom(sockfd, str_pass, sizeof(str_pass), 0,
                                        (struct sockaddr *)&servaddr, &len);
                    if (rcvBytes < 0)
                    {
                        perror("Error");
                        break;
                    }
                    str_pass[rcvBytes] = '\0';
                    printf("%s\n\n", str_pass);
                }
            }
        }
    }
    close(sockfd);
    return 1;
}
