#include <stdio.h>
#include <netinet/in.h> 
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

const int MODE_IP = 1;
const int MODE_NAME = 2;

struct hostent* getInfoByNameOrIP(int mode, char* arg){

    struct hostent* host = NULL;
    
    if(mode==MODE_NAME){
        host = gethostbyname(arg);
        if(host!=NULL){
            struct in_addr addrName;
            addrName.s_addr = *(in_addr_t*)host->h_addr_list[0];

            struct in_addr test;
            inet_aton(host->h_name, &test);

            if(addrName.s_addr == test.s_addr){
                host = NULL;
            }

        }

    }else if(mode==MODE_IP){
        struct in_addr addrIP;
        addrIP.s_addr = inet_addr(arg);
        
        if(addrIP.s_addr!=-1){
            host = gethostbyaddr(&addrIP, 4, AF_INET);
        }
    }else{
        host=NULL;
    }

    return host;
}

int printInfoName(struct hostent *host){

    struct in_addr addrName;
    addrName.s_addr = *(in_addr_t*)host->h_addr_list[0];
    printf("Official IP: %s\n", inet_ntoa(addrName));

    for(int i=1; host->h_addr_list[i]!=NULL; ++i){
        addrName.s_addr = *(in_addr_t*)host->h_addr_list[i];
        if(i==1){
            printf("Alias IP:\n");
        }
        printf("%s\n", inet_ntoa(addrName));
    }

}

int printInfoIP(struct hostent* host){

    printf("Official name: %s\n", host->h_name);

    for(int i=0; host->h_aliases[i]!=NULL; ++i){
        if(i==0){
            printf("Alias name:\n");
        }
        printf("%s\n", host->h_aliases[i]);
    }

}

int main(int argc, char** argv){

    if(argc!=3){
        printf("Not enough arguments\n");
        return -1;
    }

    if(strcmp(argv[1], "1")!=0 && strcmp(argv[1], "2")!=0){
        printf("The first argument is invalid\n");
        return -1;
    }

    struct hostent* host;
    if(strcmp(argv[1], "1")==0){
        if((host=getInfoByNameOrIP(MODE_IP, argv[2]))!=NULL){
            printInfoIP(host);
        }else if((host=getInfoByNameOrIP(MODE_NAME, argv[2]))!=NULL){
            printf("Wrong parameter\n");
        }else{
            printf("Not found information\n");
        }
    }else if(strcmp(argv[1], "2")==0){
        if((host=getInfoByNameOrIP(MODE_NAME, argv[2]))!=NULL){
            printInfoName(host);
        }else if((host=getInfoByNameOrIP(MODE_IP, argv[2]))!=NULL){
            printf("Wrong parameter\n");
        }else{
            printf("Not found information\n");
        }
    }

}