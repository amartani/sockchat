/**
 * Server for Chat
 *
 * Dalton Pinto
 */

#include "common.c"
#include <time.h>
#include <netinet/tcp.h>

#define COORDINATOR_IP "127.0.0.1"
#define COORDINATOR_PORT 5000

server_info servers[SERVER_LIST_SIZE];

// Porta: unsigned short (2 bytes)
// IP:    unsigned int   (4 bytes)

// ------- Function Prototypes --------
// void error(char *msg);
// string string_create(char *);
// string recv_string(int sock);
// void send_string(int sock, string str);
// void free_string(string str)

// ------- Commands Prototypes --------
void cmd_list_servers(int sock);
void cmd_connect(int sock);
void cmd_send_message(int sock);
void cmd_heartbeat(int sock);
void cmd_list_users(int sock);
void cmd_echo(int sock);

// ------- Server Response Prototypes --------
void server_sent_message(int sock);
void server_listed_users(int sock);
void server_echoed(int sock);

// ------- Helper Prototypes --------
int handle_user(char, int);
int handle_server(char, int);
void *listening(void *);
void *heart_beating(void *);
void send_string_command(int, char);
int get_socket(char *, int);
void choose_server(char *, int *);

// ------- Very Simple Functions --------
void sigquit(){ exit(0); }

int main(int argc, char *argv[])
{
  int server_socket, coordinator_socket, port;
  char string_ip[15], code;
  pthread_t listening_thread, beating_thread;
  void *arg;

  signal(SIGQUIT, sigquit); // Waiting to be killed (kill -QUIT <pid>)

  if(argc != 3){
    strcpy(string_ip, COORDINATOR_IP);
    port = COORDINATOR_PORT;
  }else{
    strcpy(string_ip, argv[1]);
    port = atoi(argv[2]);
  }

  coordinator_socket = get_socket(string_ip, port);
  cmd_list_servers(coordinator_socket);

  choose_server(string_ip, &port);
  server_socket = get_socket(string_ip, port);
  cmd_connect(server_socket);

  // Started Listening Thread that listens server responses
  arg = malloc(sizeof(server_socket));
  memcpy(arg, &server_socket, sizeof(server_socket));
  pthread_create(&listening_thread, 0, listening, arg);

  // Started Heartbeat Thread that keeps beating for each 5 seconds
  arg = malloc(sizeof(server_socket));
  memcpy(arg, &server_socket, sizeof(server_socket));
  pthread_create(&beating_thread, 0, heart_beating, arg);

  // Main loop that receives users commands
  while(1){
    scanf("%c", &code);
    while(code == '\n') scanf("%c", &code);
    if( handle_user(code, server_socket) ) break;
  }

  close(server_socket);
  close(coordinator_socket);

  return 0;
}


// ------- Commands Functions --------
void cmd_list_servers(int sock){
  int bytes_recieved, i;
  char send_data[1024], recv_data[1024];

  send(sock, "C", sizeof(char), 0);
  recv(sock, recv_data, sizeof(char), 0);
  if(recv_data[0] != 'C') error("ERROR Invalid Command");

  for(i = 0; i < SERVER_LIST_SIZE; i++){
    recv(sock, &(servers[i]), sizeof(server_info), 0);
    printf("<%d> IP: %d.%d.%d.%d Porta: %d\n", i, servers[i].ip[0], servers[i].ip[1], servers[i].ip[2], servers[i].ip[3], servers[i].port);
  }

  fflush(stdout);
}

void cmd_connect(int sock){
  send_string_command(sock, 'C');
}

void cmd_send_message(int sock){
  send_string_command(sock, 'M');
}

void cmd_heartbeat(int sock){
  send(sock, "H", sizeof(char), 0);
}

void cmd_list_users(int sock){
  send(sock, "L", sizeof(char), 0);
}

void cmd_echo(int sock){
  send_string_command(sock, 'E');
}

// ------- Server Responses --------
void server_sent_message(int sock){
  string str;
  str = recv_string(sock);
  printf("Message:\n%s\n", str.str);
  fflush(stdout);
}

void server_listed_users(int sock){
  int size, i;
  string str;

  recv(sock, &size, sizeof(int), 0);
  printf("Clients (%d):\n", size);

  for(i = 0; i < size; i++){
    str = recv_string(sock);
    printf("%s\n", str.str);
  }
  fflush(stdout);
}

void server_echoed(int sock){
  string str;
  str = recv_string(sock);
  printf("Echoed:\n%s\n", str.str);
  fflush(stdout);
}

// ------- Helper Functions --------
int handle_user(char code, int sock){
  switch(code){
    case 'C':
      cmd_connect(sock); break;
    case 'M':
      cmd_send_message(sock); break;
    case 'H':
      cmd_heartbeat(sock); break;
    case 'L':
      cmd_list_users(sock); break;
    case 'E':
      cmd_echo(sock); break;
    case 'Q':
      return 1;
    default:
      printf("UNKNOWN COMMAND!\n");
      fflush(stdout);
  }
  return 0;
}

int handle_server(char code, int sock){
  // printf("CODE: %c, %d\n", code, (int) code);
  switch(code){
    case 'M':
      server_sent_message(sock); break;
    case 'L':
      server_listed_users(sock); break;
    case 'E':
      server_echoed(sock); break;
    default:
      return 1;
  }
  return 0;
}

void *listening(void *arg){
  int sock = *((int*)arg);
  char code;

  free(arg);
  while(1){
    recv(sock, &code, sizeof(char), 0);
    if( handle_server(code, sock) ) error("ERROR Something went wrong...");
  }
}

void *heart_beating(void *arg){
  int sock = *((int*)arg);
  free(arg);
  while(1){
    cmd_heartbeat(sock);
    sleep(5);
  }
}

void send_string_command(int sock, char cmd){
  char str[100];
  fgets(str, 100, stdin);
  while(strcmp(str, "\n") == 0) fgets(str, 100, stdin);
  send(sock, &cmd, 1, 0);
  send_string(sock, string_create(str));
}

int get_socket(char *ip_address, int port){
  int sock;
  struct sockaddr_in coordinator_address;
  struct hostent *host;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock < 0) error("ERROR opening coordinator socket");

  host = gethostbyname(ip_address);

  coordinator_address.sin_family = AF_INET;
  coordinator_address.sin_port = htons(port);
  coordinator_address.sin_addr = *((struct in_addr *)host->h_addr);
  bzero(&(coordinator_address.sin_zero),8);
  if( connect(sock, (struct sockaddr *)&coordinator_address, sizeof(coordinator_address)) < 0 ) error("ERROR connecting");
  return sock;
}

void choose_server(char *string_ip, int *port){
  int code;

  printf("Choose server (0-9):\n");
  scanf("%d", &code);

  if(code > 9 || code < 0){ printf("\n::%d::\n", code); error("ERROR Type number between 0 or 9"); }
  sprintf(string_ip, "%d.%d.%d.%d", servers[code].ip[0], servers[code].ip[1], servers[code].ip[2], servers[code].ip[3]);
  *port = servers[code].port;

  printf("Server => IP: %s Porta: %d\n", string_ip, *port);
  fflush(stdout);
}