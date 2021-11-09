#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#define BUFF_SIZE 100

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

    int sockfd, rcvBytes, sendBytes;
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
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Error: ");
        return 0;
    }

    //Step 2: Bind address to socket
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        perror("Error: ");
        return 0;
    }
    printf("Server started\n");

    //Step 3: Communicate with client
    for (;;)
    {
        char username[30], password[30], newPass[30];
        len = sizeof(cliaddr);
        //printf("hi\n");
        if (is_login == 0)
        {
            rcvBytes = recvfrom(sockfd, username, sizeof(username), 0,
                                (struct sockaddr *)&cliaddr, &len);

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
                printf("\nCannot find account\n");
                sendto(sockfd, "Cannot find account", BUFF_SIZE, 0,
                       (struct sockaddr *)&cliaddr, len);
                continue;
            }
            if (tmp->acc.status != 1)
            {
                printf("Account not ready\n");
                sendto(sockfd, "Account not ready", BUFF_SIZE, 0,
                       (struct sockaddr *)&cliaddr, len);
            }
            else
            {
                printf("Insert password!\n");
                sendto(sockfd, "Insert password", BUFF_SIZE, 0,
                       (struct sockaddr *)&cliaddr, len);
                int count = 0;
                while (1)
                {
                    rcvBytes = recvfrom(sockfd, password, sizeof(password), 0,
                                        (struct sockaddr *)&cliaddr, &len);
                    if (rcvBytes < 0)
                    {
                        perror("Error: ");
                        return 0;
                    }
                    password[rcvBytes] = '\0';
                    if (strcmp(tmp->acc.password, password) == 0)
                    {
                        printf("OK\n");
                        sendto(sockfd, "OK", BUFF_SIZE, 0,
                               (struct sockaddr *)&cliaddr, len);
                        strcpy(userSignedIn, username);
                        is_login = 1;
                        break;
                    }
                    if (count < 2)
                    {
                        printf("Not OK\n");
                        sendto(sockfd, "Not OK", BUFF_SIZE, 0,
                               (struct sockaddr *)&cliaddr, len);
                        count++;
                    }
                    else
                    {
                        tmp->acc.status = 0;
                        writeFile();
                        printf("Account is blocked\n");
                        sendto(sockfd, "Account is blocked", BUFF_SIZE, 0,
                               (struct sockaddr *)&cliaddr, len);
                        break;
                    }
                }
            }
        }
        if (is_login == 1)
        {
            node *temp = checkUsername(userSignedIn);
            rcvBytes = recvfrom(sockfd, newPass, sizeof(newPass), 0,
                                (struct sockaddr *)&cliaddr, &len);
            if (rcvBytes < 0)
            {
                perror("Error: ");
                return 0;
            }
            newPass[rcvBytes] = '\0';
            if (strcmp(newPass, "bye") == 0)
            {
                is_login = 0;
                printf("%s %s\n","Goodbye", userSignedIn);
                sendto(sockfd, "Goodbye", BUFF_SIZE, 0,
                       (struct sockaddr *)&cliaddr, len);
                strcpy(userSignedIn, "");
                //break;
            }
            else
            {
                char number_pass[10], str_pass[10];
                if (encode_pass(newPass, number_pass, str_pass) == 1)
                {
                    sendto(sockfd, number_pass, strlen(number_pass), 0,
                           (struct sockaddr *)&cliaddr, len);
                    sendto(sockfd, str_pass, strlen(str_pass), 0,
                           (struct sockaddr *)&cliaddr, len);
                    strcpy(temp->acc.password, newPass);
                    writeFile();
                }else{
                    printf("Error\n");
                    sendto(sockfd, "Error", BUFF_SIZE, 0,
                           (struct sockaddr *)&cliaddr, len);
                }
            }
        }
    }
    close(sockfd);
    return 1;
}
