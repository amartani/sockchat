/**
 * Server for Chat
 *
 * Alexandre Martani <amartani at gmail com>
 *
 * References:
 * - Threads: http://www.cs.cf.ac.uk/Dave/C/node32.html
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

// structs

struct string {
    uint size;
    char *str;
};

typedef struct string string;

// function prototypes

void error(char *msg);
string recv_string(int sock);
void send_string(int sock, string str);

void *do_chld(void *arg);
void select_command(int sock);
void client_handle(int sock);

// commands

void cmd_list(int sock);
void cmd_echo(int sock);
void cmd_unknown(int sock);

// helper functions

void error(char *msg)
{
    perror(msg);
    exit(1);
}

// helper function to send and receive string structs

string recv_string(int sock)
{
    string result;
    int n;
    n = read(sock, &result.size, sizeof(result.size));
    if (n < 0) error("ERROR");
    result.str = (char*) malloc(result.size*sizeof(char));
    n = read(sock, result.str, result.size*sizeof(char));
    if (n < 0) error("ERROR");
    return result;
}

void send_string(int sock, string str)
{
    int n;
    n = write(sock, &str.size, sizeof(str.size));
    if (n < 0) error("ERROR");
    n = write(sock, str.str, str.size*sizeof(char));
    if (n < 0) error("ERROR");
}

void free_string(string str)
{
    free(str.str);
}

// main function

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    pthread_t chld_thr;

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
            sizeof(serv_addr)) < 0)
            error("ERROR on binding");
    listen(sockfd, 5);

    // Loop forever accepting connections
    while (1) {
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd,
                           (struct sockaddr *) &cli_addr,
                           &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");

        // Create new thread
        pthread_create(&chld_thr, 0, do_chld, (void*) &newsockfd);
    }
    return 0;
}

// New threads calls this
void *do_chld(void *arg)
{
    int sock = *((int*)arg);

    client_handle(sock);
}

// Handle a connection with a client

void client_handle(int sock)
{
    while (1) {
        select_command(sock);
    }
}

// Listen to a command and execute
void select_command(int sock)
{
    int n;
    char command;

    n = read(sock, &command, sizeof(command));
    if (n < 0) error("ERROR reading from socket");

    switch (command) {
    case 'L':
        cmd_list(sock);
        break;
    case 'E':
        cmd_echo(sock);
        break;
    default:
        cmd_unknown(sock);
        break;
    }
}

// commands

void cmd_list(int sock)
{
    int n;

    n = write(sock, "Comando L",9);
    if (n < 0) error("ERROR writing to socket");
}

void cmd_echo(int sock)
{
    string str;

    str = recv_string(sock);
    send_string(sock, str);
}

void cmd_unknown(int sock)
{
    int n;

    n = write(sock,"Desconhecido",12);
    if (n < 0) error("ERROR writing to socket");
}

