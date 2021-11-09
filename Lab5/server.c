#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#define BUFF_SIZE 100
#define BACKLOG 1 //number of pending connections in queue

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

int checknum_str(char *newpass)
{
    int len = strlen(newpass);
    if (len == 0)
        return 0;
    for (int i = 0; i < len; i++)
    {
        if (!((newpass[i] >= '0' && newpass[i] <= '9') ||
              (newpass[i] >= 'a' && newpass[i] <= 'z') ||
              (newpass[i] >= 'A' && newpass[i] <= 'Z')))
            return 0;
    }
    return 1;
}

int encode_pass(char *newpass, char *number_pass, char *str_pass)
{
    int m = 0, n = 0;
    int len = strlen(newpass);
    if (len == 0)
    {
        number_pass = "";
        str_pass = "";
        return 0;
    }
    if (checknum_str(newpass) == 0)
        return 0;

    for (int i = 0; i < len; i++)
    {
        if (newpass[i] >= '0' && newpass[i] <= '9')
        {
            number_pass[m++] = newpass[i];
        }
        else
            str_pass[n++] = newpass[i];
    }
    number_pass[m] = '\0';
    str_pass[n] = '\0';
    return 1;
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
    int rcvBytes, sendBytes;
    int is_login = 0;
    socklen_t len;
    char buff[BUFF_SIZE + 1];
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

    printf("Server started\n");

    //Step 4: Communicate with client
    for (;;)
    {
        char username[30], password[30], newPass[30];
        len = sizeof(cliaddr);
        //printf("hi\n");
        if ((conn_sock = accept(listen_sock, (struct sockaddr *)&cliaddr, &len)) == -1)
        {
            perror("\nError: ");
        }
        printf("You got a connection from %s\n", inet_ntoa(cliaddr.sin_addr));

        while (1)
        {
            if (is_login == 0)
            {
                rcvBytes = recv(conn_sock, username, sizeof(username), 0);
                if (rcvBytes < 0)
                {
                    perror("Error: ");
                    return 0;
                }
                username[rcvBytes] = '\0';
                //printf("%s",username);
                node *tmp = checkUsername(username);
                if (tmp == NULL)
                {
                    strcpy(buff,"Cannot find account");
                    printf("%s\n",buff);
                    send(conn_sock, buff, strlen(buff), 0);
                    continue;
                }
                if (tmp->acc.status != 1)
                {
                    strcpy(buff,"Account not ready");
                    printf("%s\n",buff);
                    send(conn_sock, buff, strlen(buff), 0);
                }
                else
                {
                    strcpy(buff,"Insert password");
                    printf("%s\n",buff);
                    send(conn_sock, buff, strlen(buff), 0);
                    int count = 0;
                    while (1)
                    {
                        rcvBytes = recv(conn_sock, password, sizeof(password), 0);
                        if (rcvBytes < 0)
                        {
                            perror("Error: ");
                            return 0;
                        }
                        password[rcvBytes] = '\0';
                        if (strcmp(tmp->acc.password, password) == 0)
                        {
                            strcpy(buff,"OK");
                            printf("%s\n",buff);
                            send(conn_sock, buff, strlen(buff), 0);
                            strcpy(userSignedIn, username);
                            is_login = 1;
                            break;
                        }
                        if (count < 2)
                        {
                            strcpy(buff,"Not OK");
                            printf("%s\n",buff);
                            send(conn_sock, buff, strlen(buff), 0);
                            count++;
                        }
                        else
                        {
                            tmp->acc.status = 0;
                            writeFile();
                            strcpy(buff,"Account is blocked");
                            printf("%s\n",buff);
                            send(conn_sock, buff, strlen(buff), 0);
                            break;
                        }
                    }
                }
            }
            if (is_login == 1)
            {
                node *temp = checkUsername(userSignedIn);
                rcvBytes = recv(conn_sock, newPass, sizeof(newPass), 0);

                if (rcvBytes < 0)
                {
                    perror("Error: ");
                    return 0;
                }
                newPass[rcvBytes] = '\0';
                if (strcmp(newPass, "bye") == 0)
                {
                    is_login = 0;
                    strcpy(buff,"Goodbye ");
                    strcat(buff,userSignedIn);
                    printf("%s\n",buff);
                    send(conn_sock, buff, strlen(buff), 0);
                    strcpy(userSignedIn, "");
                    //break;
                }
                else
                {
                    char number_pass[10], str_pass[10];
                    if (encode_pass(newPass, number_pass, str_pass) == 1)
                    {
                        strcpy(buff,number_pass);
                        strcat(buff,"\n");
                        strcat(buff,str_pass);
                        printf("%s\n",buff);
                        send(conn_sock, buff, strlen(buff), 0);
                        strcpy(temp->acc.password, newPass);
                        writeFile();
                    }
                    else
                    {
                        printf("Error\n");
                        strcpy(buff,"Error");
                        send(conn_sock, buff, strlen(buff), 0);
                    }
                }
            }
        }
        close(conn_sock);
    }
    close(listen_sock);
    return 1;
}
