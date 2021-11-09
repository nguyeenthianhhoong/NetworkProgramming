#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<netdb.h>
#include<arpa/inet.h>

typedef struct account{
    char username[30];
    char password[30];
    int status;
    //int isLogin;
    char homepage[30];
}account;

typedef account account;

typedef struct node{
    account acc;
    struct node *next;
}node;

typedef struct node node;
node *root, *cur;
char userSignedIn[30];

//
node *makeNewNode(account);
void addAcount(account);
void readFile(FILE *);
void writeFile();
void trim(char[]);
int checkSpace(char[]);
void display();
void freeList(node *);
node *checkUsername(char[]);

void regis();
void active();
void signIn();
void search();
void changePass();
void signOut();
int check(char *);
void getInfoByIP();
void getInfoByName();

int main(){
    char choice;
    //int choice;
    FILE *f;
    f=fopen("account.txt","r");
    if(f==NULL){
        printf("Can't open file account.txt\n");
        return 0;
    }
    readFile(f);
    //display();
    do{
        printf("\n\n\tUSER MANAGEMENT PROGRAM\n");
        printf("---------------------------------------\n");
        printf("1. Register\n");
        printf("2. Active\n");
        printf("3. Sign in\n");
        printf("4. Search\n");
        printf("5. Change password\n");
        printf("6. Sign out\n");
        printf("7. Homepage with domain name\n");
        printf("8. Homepage with IP address\n");
        printf("Your choice (1-6, other to quit): ");
        scanf("%c",&choice);
        //choice = getchar();
        switch(choice){
            case '1':
                getchar();
                regis();
                break;

            case '2':
                getchar();
                active();
                //display();
                break;

            case '3':
                getchar();
                signIn();
                //display();
                break;
            
            case '4':
                //display();
                getchar();
                search();
                break;
            
            case '5':
                //display();
                getchar();
                changePass();
                break;

            case '6':
                //display();
                getchar();
                signOut();
                break;

            case '7': 
                getchar();
                getInfoByIP();
                break;

            case '8':
                getchar();
                getInfoByName();
                break;
            default:
                break;
        }
    }while(choice > '0' && choice < '9');
    freeList(root);
    return 0;
}


node *makeNewNode(account acc){
    node *new = (node*)malloc(sizeof(node));
    new->acc = acc;
    new->next = NULL;
    return new;
}

// void addAccount(account acc){        //at head
//     node *new = makeNewNode(acc);
//     if( root == NULL){
//       root = new;
//       cur = root;
//     }else{
//     new ->next = root;
//     root = new;
//     cur = root;
//     }
// }

void addAccount(account acc){       //at end
    node *new = makeNewNode(acc);
    if( root == NULL){
      root = new;
    }else{
        cur = root;
        while(cur->next != NULL){
            cur = cur->next;
        }
        cur->next = new;
    }
}

void readFile(FILE *f){
    account acc ;
    while(!feof(f)){
        fscanf(f,"%s %s %d %s\n",acc.username,acc.password,&acc.status,acc.homepage);
        addAccount(acc);
    }
    fclose(f);
}

void writeFile(){
    FILE *f;
    node *tmp = root;
    f = fopen("account.txt","w");
    while(tmp != NULL){
        fprintf(f,"%s %s %d %s\n",tmp->acc.username,tmp->acc.password,tmp->acc.status,tmp->acc.homepage);
        tmp = tmp->next;
    }
    fclose(f);
}

void trim(char s[]){
    int i = strlen(s);
    while(s[0] == ' ' ) {
        for(int j=0; j<i; j++)
            s[j]= s[j+1];
        i--;
    }
    while(s[i-1] == ' ') 
        i--;
    s[i] = '\0';
}

int checkSpace(char s[]){
    for(int i=0;i<strlen(s);i++){
        if(s[i]== ' ')
            return 1;
    }
    return 0;
}

void display(){
    node *tmp = root;
    while(tmp != NULL){
        printf("%s %s %d %s\n", tmp->acc.username, tmp->acc.password, tmp->acc.status, tmp->acc.homepage);
        tmp = tmp->next;
    }    
}

void freeList(node *root){
  node *tmp = root;
  while(tmp != NULL)
    {
      root = root->next;
      free(tmp);
      tmp = root;
    }
}

node *checkUsername(char username[]){
    node *tmp = root;
    while(tmp != NULL){
        if(strcmp(tmp->acc.username,username) == 0)
            return tmp;
        tmp = tmp->next;
    }
    return NULL;
}

void regis(){
    account acc;
    printf("\n--------\nREGISTER\n--------\n");
    do{
        printf("Enter username: ");
        gets(acc.username);
        trim(acc.username);
        if(checkSpace(acc.username) == 1 || strlen(acc.username) == 0){
            printf("\nInvalid username.(Username does not include spaces)\nPlease try again\n");
        }else if(checkUsername(acc.username) != NULL)
            printf("\nAccount existed\n");
        else
            break;
    }while(1);
    do{
        printf("Enter password: ");
        gets(acc.password);
        if(checkSpace(acc.password) == 1 || strlen(acc.username) == 0){
            printf("\nInvalid password.(Password does not include spaces)\nPlease try again\n");
        }else
            break;
    }while(1);
    printf("Enter homepage: ");
    gets(acc.homepage);
    if(checkSpace(acc.homepage) == 1 || strlen(acc.homepage) == 0){
            printf("\nInvalid password\n");
            return;
    }
    acc.status = 2;
    addAccount(acc);
    writeFile();
    printf("\nSuccessful registration. Activation required\n");
    //display();
}

void active(){  
    node *tmp; 
    char username[30];
    char password[30];
    int count = 0;
    printf("\n--------\nACTIVATE\n--------\n");
    printf("Enter username: ");
    gets(username);
    trim(username);
    tmp = checkUsername(username);
    if(tmp == NULL){
        printf("\nCannot find account\n");
        return;
    }
    int check = tmp->acc.status;
    if(check != 2){
        printf("\nAccount is activated\n");
        return;
    }
    printf("Enter password: ");
    gets(password);
    if(strcmp(tmp->acc.password, password) != 0){
        printf("\nPassword is incorrect\n");
        return;
    }
    char code[8]; 
    do{
        printf("Enter code: ");
        //scanf("%s",code);
        gets(code);
        if(strcmp(code, "20184112") == 0){ 
            tmp->acc.status = 1;
            writeFile();
            printf("\nAccount is activated\n");
            return;            
        }else {
            printf("\nAcount is not activated\n");
            count++;
        }
    }while (count<4);
    tmp->acc.status = 0;
    writeFile();
    printf("\nActivation code is incorrect. Account is blocked\n");
    return;
}

void signIn(){
    char username[30];
    char password[30];
    int count = 0;
    if(strlen(userSignedIn) != 0){
        printf("\nPlease sign out your current account (%s).\n",userSignedIn);
        return;
    }
    printf("\n--------\nSIGN IN\n--------\n");
    printf("Enter username: ");
    gets(username);
    trim(username);
    node *tmp = checkUsername(username);
    if(tmp == NULL){
        printf("\nCannot find account\n");
        return;
    }
    int check = tmp->acc.status;
    if(check == 0){
        printf("\nAccount is blocked\n");
        return;
    }else if(check == 2){
        printf("\nAccount is not activated\n");
        return;
    }else{
        do{
            printf("Enter password: ");
            gets(password);
            if(strcmp(tmp->acc.password,password) != 0){
                printf("\nPassword is incorrect\n");
                count++;
            }else{
                strcpy(userSignedIn,username);
                printf("\nHello %s\n",username);
                return;
            }
        }while (count<3);
        tmp->acc.status = 0;
        writeFile();
        printf("\nPassword is incorrect. Account is blocked\n");
    }
    return;
}

void search(){
    char name[30];
    printf("\n--------\nSEARCH\n--------\n");
    printf("Enter username: ");
    gets(name);
    trim(name);
    node *tmp = checkUsername(name);
    if(tmp == NULL){
        printf("\nCannot find account\n");
        return;
    }
    int check = tmp->acc.status;
    if(check == 0){
        printf("\nAccount is blocked\n");
        return;
    }else if(check == 2){
        printf("\nAccount is not activated\n");
        return;
    }else printf("\nAccount is active\n");
    return;
}

// void changePass(){
//     char username[30];
//     char password[30];
//     printf("\n--------\nCHANGE PASSWORD\n--------\n");
//     printf("Enter username: ");
//     gets(username);
//     trim(username);
//     node *tmp = checkUsername(username);
//     if(tmp == NULL){
//         printf("\nCannot find account\n");
//         return;
//     }
//     if(tmp->acc.isLogin == 0){
//         printf("\nAccount is not sign in\n");
//         return;
//     }
//     printf("Enter password: ");
//     gets(password);
//     while(strcmp(tmp->acc.password,password) != 0){
//         printf("\nCurrent password is incorrect. Please try again\n\n");
//         printf("Enter password: ");
//         gets(password);
//     }
//     do{
//         printf("Enter Newpassword: ");
//         gets(tmp->acc.password);
//         if(checkSpace(tmp->acc.password) == 1 || strlen(tmp->acc.password) == 0){
//             printf("\nInvalid password.(Password does not include spaces)\nPlease try again\n");
//         }else if(strcmp(tmp->acc.password,password) == 0){
//             printf("\nPassword is not changed\n");
//             return;
//         }else
//             break;
//     }while(1);       
//     writeFile();
//     printf("\nPassword is changed\n");
//     return;
// }

void changePass(){
    char username[30];
    char password[30];
    char newPass[30];
    printf("\n--------\nCHANGE PASSWORD\n--------\n");
    do{
        printf("Enter username: ");
        gets(username);
        trim(username);
        node *tmp = checkUsername(username);
        if(tmp == NULL){
            printf("\nCannot find account\n");
            return;
        }
        if(strcmp(username,userSignedIn) != 0){
            printf("\nAccount is not sign in\n");
            return;
        }
        printf("Enter password: ");
        gets(password);
        printf("Enter Newpassword: ");
        gets(newPass);
        if(strcmp(tmp->acc.password,password) != 0){
            printf("\nCurrent password is incorrect. Please try again\n\n");
        }else if(checkSpace(newPass) == 1 || strlen(newPass) == 0){
            printf("\nInvalid password.(Password does not include spaces)\nPlease try again\n");
        }else if(strcmp(tmp->acc.password,newPass) == 0){
            printf("\nPassword is not changed\n");
            return;
        }else{
            strcpy(tmp->acc.password,newPass);
            break;
        }
    }while(1);       
    writeFile();
    printf("\nPassword is changed\n");
    return;
}

void signOut(){
    char username[30];
    printf("\n--------\nSIGN OUT\n--------\n");
    printf("Enter username: ");
    gets(username);
    trim(username);
    node *tmp = checkUsername(username);
    if(tmp == NULL){
        printf("\nCannot find account\n");
        return;
    }
    if(strcmp(username,userSignedIn) != 0){
        printf("\nAccount is not sign in\n");
        return;
    }
    strcpy(userSignedIn,"");
    printf("\nGoodbye %s\n",username);
}

int check(char *str){
    while(*str != '\0')
    {
        if(isxdigit(*str) == 0)  // hexa
            if(*str != '.') 
                return 0; // hostname
        str++;
    }
    return 1; // IP address
}

void getInfoByIP(){
    node *tmp = checkUsername(userSignedIn);
    if(tmp == NULL){
        printf("\nAccount is not sign in\n");
        return;
    }
    if(check(tmp->acc.homepage) == 0){
        printf("\nOfficial name: %s\n",tmp->acc.homepage);
        if(gethostbyname(tmp->acc.homepage) == NULL){
            printf("Not found infomation\n");
        }
    }else{
        struct in_addr addr;
        addr.s_addr = inet_addr(tmp->acc.homepage); //convert string pointed to an integer value
        //printf("%d\n",addr.s_addr);
        if(addr.s_addr == -1){ 
            printf("Not found infomation\n");
        }else{
            struct hostent *host = gethostbyaddr(&addr,4,AF_INET);
            if(host == NULL){
                printf("\nNot found infomation\n");
                return;
            }else{
                printf("\nOfficial name: %s\n",host->h_name);
            }
        }
    }
    return;
}

void getInfoByName(){
    node *tmp = checkUsername(userSignedIn);
    if(tmp == NULL){
        printf("\nAccount is not sign in\n");
        return;
    }
    if(check(tmp->acc.homepage) == 1){
        printf("\nOfficial IP: %s\n",tmp->acc.homepage);
        if(inet_addr(tmp->acc.homepage) == -1){
            printf("Not found infomation\n");
        }
    }else{
        struct hostent *host = gethostbyname(tmp->acc.homepage);
        if(host == NULL){
            printf("\nNot found infomation\n");
            return;
        }else{
            printf("\nOfficial IP: %s\n", inet_ntoa(*((struct in_addr *)host->h_addr)));
        }
    }
    return;
}


