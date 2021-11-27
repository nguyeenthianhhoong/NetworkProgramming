#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#define BUFF_SIZE 100
#define BACKLOG 5 //number of pending connections in queue

typedef struct account
{
    char username[30];
    char password[30];
    int status;
    //int isLogin;
    //char homepage[30];
} account;

typedef account account;

typedef struct node
{
    account acc;
    struct node *next;
} node;

typedef struct node node;
node *root, *cur;
char userSignedIn[30];

node *makeNewNode(account acc)
{
    node *new = (node *)malloc(sizeof(node));
    new->acc = acc;
    new->next = NULL;
    return new;
}

void addAccount(account acc)
{ //at end
    node *new = makeNewNode(acc);
    if (root == NULL)
    {
        root = new;
    }
    else
    {
        cur = root;
        while (cur->next != NULL)
        {
            cur = cur->next;
        }
        cur->next = new;
    }
}

void readFile(FILE *f)
{
    account acc;
    while (!feof(f))
    {
        fscanf(f, "%s %s %d\n", acc.username, acc.password, &acc.status);
        addAccount(acc);
    }
    fclose(f);
}

void writeFile()
{
    FILE *f;
    node *tmp = root;
    f = fopen("account.txt", "w");
    while (tmp != NULL)
    {
        fprintf(f, "%s %s %d\n", tmp->acc.username, tmp->acc.password, tmp->acc.status);
        tmp = tmp->next;
    }
    fclose(f);
}

node *checkUsername(char username[])
{
    node *tmp = root;
    while (tmp != NULL)
    {
        if (strcmp(tmp->acc.username, username) == 0)
            return tmp;
        tmp = tmp->next;
    }
    return NULL;
}

void *client_handler(void *arg)
{
    int clientfd;
    int sendBytes, rcvBytes;
    int is_login = 0;
    char buff[BUFF_SIZE];
    char username[30], password[30];
    pthread_detach(pthread_self());
    clientfd = *((int*) arg);
    while (1)
    {
        rcvBytes = recv(clientfd, username, sizeof(username), 0);
        if (rcvBytes < 0)
        {
            perror("Error: ");
            break;
        }
        username[rcvBytes] = '\0';
        if(strlen(username) == 0){
            strcpy(buff, "Goodbye");
            send(clientfd, buff, strlen(buff), 0);
            break;
        }
        if (strcmp(username, "bye") == 0)
        {
            is_login = 0;
            strcpy(buff, "Please log in");
            printf("%s\n", buff);
            send(clientfd, buff, strlen(buff), 0);
            continue;
        }
        if (is_login == 0)
        {
            node *tmp = checkUsername(username);
            if (tmp == NULL)
            {
                strcpy(buff, "Cannot find account");
                printf("%s\n", buff);
                send(clientfd, buff, strlen(buff), 0);
            }
            else if (tmp->acc.status != 1)
            {
                strcpy(buff, "Account not ready");
                printf("%s\n", buff);
                send(clientfd, buff, strlen(buff), 0);
            }
            else
            {
                strcpy(buff, "Insert password");
                printf("%s\n", buff);
                send(clientfd, buff, strlen(buff), 0);
                int count = 0;
                while (1)
                {
                    rcvBytes = recv(clientfd, password, sizeof(password), 0);
                    if (rcvBytes < 0)
                    {
                        perror("Error: ");
                        break;
                    }
                    password[rcvBytes] = '\0';
                    if (strcmp(tmp->acc.password, password) == 0)
                    {
                        strcpy(buff, "OK");
                        printf("%s\n", buff);
                        send(clientfd, buff, strlen(buff), 0);
                        strcpy(userSignedIn, username);
                        is_login = 1;
                        break;
                    }
                    if (count < 2)
                    {
                        strcpy(buff, "Not OK");
                        printf("%s\n", buff);
                        send(clientfd, buff, strlen(buff), 0);
                        count++;
                    }
                    else
                    {
                        tmp->acc.status = 0;
                        writeFile();
                        strcpy(buff, "Account is blocked");
                        printf("%s\n", buff);
                        send(clientfd, buff, strlen(buff), 0);
                        break;
                    }
                }
            }
        }
        else
        {
            strcpy(buff, "Please log out current account");
            printf("%s\n", buff);
            send(clientfd, buff, strlen(buff), 0);
        }
    }
    close(clientfd);
}


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Wrong num of arguments!\n");
        printf("correct syntax: \"server <Port number> \"\n");
        return 1;
    }
    FILE *f;
    f = fopen("account.txt", "r");
    if (f == NULL)
    {
        printf("Can't open file account.txt\n");
        return 0;
    }
    readFile(f);

    int listen_sock, conn_sock;  
    socklen_t len;
    struct sockaddr_in servaddr, cliaddr;

    if (atoi(argv[1]) < 0 && atoi(argv[1]) > 65535)
    {
        printf("[%s] is a invalid Port number\n", argv[1]);
        exit(1);
    }

    //Step 1: Construct socket
    if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Error: ");
        return 0;
    }

    //Step 2: Bind address to socket
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));
    if (bind(listen_sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        perror("Error: ");
        return 0;
    }

    //Step 3: Listen request from client
    if (listen(listen_sock, BACKLOG) == -1)
    { /* calls listen() */
        perror("\nError: ");
        return 0;
    }

    pthread_t tid;
    printf("Server started\n");

    //Step 4: Communicate with client
    for (;;)
    {
        len = sizeof(cliaddr);
        //printf("hi\n");
        if ((conn_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &len)) == -1)
        {
            perror("\nError: ");
            break;
        }
        //printf("You got a connection from %s\n", inet_ntoa(cliaddr.sin_addr));

        pthread_create(&tid, NULL, &client_handler, (void *)&conn_sock);

    }
    close(listen_sock);
    return 1;
}

