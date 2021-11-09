#include<stdio.h>
#include<stdlib.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<ctype.h>

void getInfoByIP(char *ip){
    struct in_addr addr;
    addr.s_addr = inet_addr(ip); //convert string pointed to an integer value
    //printf("%d\n",addr.s_addr);
    if(addr.s_addr == -1){ 
        printf("Not found infomation\n");
    }else{
        struct hostent *host = gethostbyaddr(&addr,4,AF_INET);
        if(host == NULL){
            printf("Not found infomation\n");
            return;
        }else{
            printf("Official name: %s\n",host->h_name);
            printf("Alias name: \n");
            for(int i=0;host->h_aliases[i]!= NULL;i++){
                printf("%s\n",host->h_aliases[i]);
            }
        }
    }
    return;
}

void getInfoByName(char *name){
    struct hostent *host = gethostbyname(name);
    if(host == NULL){
            printf("Not found infomation\n");
            return;
    }else{
        printf("Official IP: %s\n", inet_ntoa(*((struct in_addr *)host->h_addr)));
        printf("Alias IP: \n");
        for(int i=1;host->h_addr_list[i]!= NULL;i++){
            printf("%s\n",inet_ntoa(*(struct in_addr*)(host->h_addr_list[i])));
        }
    }
    return;
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

int main(int argc, char **argv){
    if(argc != 3){
        printf("Wrong num of arguments!\n");
        printf("correct syntax: \"resolver 1 <IP address>\" or \"resolve 2 <domain name>\"\n");
        return 1;
    }
    if(atoi(argv[1]) == 1){
        if(check(argv[2]) == 0){
            printf("Wrong parameter\n");
            return 1;
        }
        getInfoByIP(argv[2]);
        
    }else if(atoi(argv[1]) == 2){
        if(check(argv[2]) == 1){
            printf("Wrong parameter\n");
            return 1;
        }
        getInfoByName(argv[2]);
    }
    return 1;

}