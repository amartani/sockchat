/**
 * Common helper functions for sockchat
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <netdb.h>
#include <errno.h>

#define SERVER_LIST_SIZE 10

// ------- String --------
struct string {
  unsigned int size;
  char *str;
};
typedef struct string string;

struct server_info {
  unsigned int ip[4];
  int port;
};
typedef struct server_info server_info;

// ------- Function Prototypes --------
void error(char *msg);
string string_create(char *);
string recv_string(int sock);
void send_string(int sock, string str);
void free_string(string str);
void send_forced(int sock, void *buf, size_t size);
void recv_forced(int sock, void *buf, size_t size);

// ------- Generic helper functions --------

void error(char *msg)
{
    perror(msg);
    exit(1);
}

// helper function to send and receive string structs

string string_create(char *char_vector){
    string str;
    int len;

    len = strlen(char_vector);
    if (char_vector[len-1] == '\n') {
        char_vector[len-1] = '\0';
        len--;
    }

    str.size = len;
    str.str = (char*) malloc(len*sizeof(char));
    strcpy(str.str, char_vector);
    return str;
}

string recv_string(int sock)
{
    string result;

    recv_forced(sock, &result.size, sizeof(result.size));

    result.str = (char*) malloc((result.size+1)*sizeof(char));
    recv_forced(sock, result.str, result.size*sizeof(char));
    result.str[result.size] = '\0';

    return result;
}

void send_string(int sock, string str)
{
    send_forced(sock, &str.size, sizeof(str.size));
    send_forced(sock, str.str, str.size*sizeof(char));
}

void free_string(string str)
{
    if (str.str != NULL)
    {
        free(str.str);
    }
}

void send_forced(int sock, void *buf, size_t size)
{
    size_t transfered;

    while (1) {
        transfered = send(sock, buf, size, 0);
        if (transfered == size) break;

        size -= transfered;
        buf += transfered;
        switch (errno) {
            case EAGAIN:
                continue;
            default:
                error("ERROR on send_forced");
        }
    }
}

void recv_forced(int sock, void *buf, size_t size)
{
    size_t transfered;

    while (1) {
        transfered = recv(sock, buf, size, 0);
        if (transfered == size) break;

        size -= transfered;
        buf += transfered;
        switch (errno) {
            case EAGAIN:
                continue;
            default:
                error("ERROR on recv_forced");
        }
    }
}
