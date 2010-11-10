/**
 * Coordinator for Chat
 *
 * Vinicius Battagello <battagello at gmail com>
 */


#include <unistd.h>     
#include "common.c"

#define COORDINATOR_IP "127.0.0.1"
#define COORDINATOR_PORT 5000

#define MAX_LISTEN 10

int dostuff(int socket, int TheValue);

void error(char * msg);



int main(int argc, char ** argv)
{
    int TheValue = 0;

    unsigned int remote_serv; // socket descriptor do servidor
    unsigned int remote_cli; // socket descriptor do cliente
    unsigned int local_s; // socket descriptor local    
    struct sockaddr_in serv_addr; // endereço da internet do servidor
    struct sockaddr_in cli_addr; // endereço da internet do cliente
    serv_list = server_info[MAX_LISTEN]; 
    
    if(argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0) {
       error("ERROR opening socket");
    }

    memset((char * ) &serv_addr, 0, sizeof(serv_addr));
    //int portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    //serv_addr.sin_port = htons(portno);
    serv_addr.sin_port = htons(COORDINATOR_PORT);

    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
       error("ERRO ao conectar");
    }

    //listen(sockfd,5);
    listen(sockfd,MAX_LISTEN);
    int clilen = sizeof(cli_addr);

    while(1) {
       int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

       if(newsockfd < 0) {
          error("ERRO na aceitacao");
       }

       TheValue = dostuff(newsockfd, TheValue);
       close(newsockfd);
    }

    return 0;
}

int dostuff(int sock, int TheValue) {
    int n = 0;
    char buffer[256];
    char valstr[256];

    memset(buffer, 0, 256);
    memset(valstr, 0, 256);

    sprintf(valstr, "%d", TheValue);
    n = write(sock, valstr, 256);

    if(n < 0) {
       error("ERRO na leitura do socket");
    }

    n = read(sock, buffer, 255);
    TheValue = atoi(buffer);

    if(n < 0) {
       error("ERRO na escrita do socket");
    }

    return TheValue;
}

void error(char * msg) {
    perror(msg);
    exit(1);
}
