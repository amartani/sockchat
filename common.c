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

// ------- Generic helper functions --------

void error(char *msg);

void error(char *msg)
{
    perror(msg);
    exit(1);
}


// ------- String --------

struct string {
    unsigned int size;
    char *str;
};

typedef struct string string;

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

