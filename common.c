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

// ------- Generic helper functions --------

void error(char *msg)
{
    perror(msg);
    exit(1);
}

// helper function to send and receive string structs

string string_create(char *char_vector){
  string str;
  char_vector[strlen(char_vector)-1] = '\0';
  str.size = strlen(char_vector);
  str.str = (char*) malloc(str.size*sizeof(char));
  strcpy(str.str, char_vector);
  return str;
}

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

