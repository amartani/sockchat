/**
 * Server for Chat
 *
 * Dalton Pinto
 */

#include "common.c"

#define COORDINATOR_IP "127.0.0.1"
#define COORDINATOR_PORT 5000

server_info servers[SERVER_LIST_SIZE];

// Porta: unsigned short (2 bytes)
// IP:    unsigned int   (4 bytes)

// ------- Function Prototypes --------
// void error(char *msg);
// string recv_string(int sock);
// void send_string(int sock, string str);
// void free_string(string str)

// ------- Commands --------
void cmd_list_servers(int sock);
// void cmd_conect(int sock);
// void cmd_send_message(int sock);
// void cmd_heartbeat(int sock);
// void cmd_list_users(int sock);
// void cmd_echo(int sock);
// void cmd_unknown(int sock);
// void cmd_receive_message(int sock);

// ------- Helper Functions --------
void command_check(char, char);

void sigquit(){ exit(0); }

int get_coordinator_socket(char *ip_address, int port){
  int sock;
  struct sockaddr_in coordinator_address;
  struct hostent *host;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if(sock < 0) error("ERROR opening coordinator socket");

  host = gethostbyname(ip_address);

  coordinator_address.sin_family = AF_INET;
  coordinator_address.sin_port = htons(port);
  coordinator_address.sin_addr = *((struct in_addr *)host->h_addr);
  // printf("(C) IP: %u.%u.%u.%u Porta: %d\n", (unsigned int) *(host->h_addr), (unsigned int) *(host->h_addr+1), (unsigned int) *(host->h_addr+2), (unsigned int) *(host->h_addr+3), (int) coordinator_address.sin_port);
  bzero(&(coordinator_address.sin_zero),8);
  if( connect(sock, (struct sockaddr *)&coordinator_address, sizeof(coordinator_address)) < 0 ) error("ERROR connecting");
  return sock;
}

void choose_server(char *string_ip, int *port){
  int code;
  printf("Choose server (0-9):\n");
  scanf("%d", &code);
  if(code > 9 || code < 0) error("ERROR Type number between 0 or 9");
  sprintf(string_ip, "%d.%d.%d.%d", servers[code].ip[0], servers[code].ip[1], servers[code].ip[2], servers[code].ip[3]);
  *port = servers[code].port;
}

int main(int argc, char *argv[])
{
  int server_socket, coordinator_socket, port;
  char string_ip[15];

  signal(SIGQUIT, sigquit); // Waiting to be killed (kill -QUIT <pid>)

  if(argc != 3){
    strcpy(string_ip, COORDINATOR_IP);
    port = COORDINATOR_PORT;
  }else{
    strcpy(string_ip, argv[1]);
    port = atoi(argv[2]);
  }

  coordinator_socket = get_coordinator_socket(string_ip, port);

  cmd_list_servers(coordinator_socket);

  choose_server(string_ip, &port);
  printf("IP: %s Porta: %d\n", string_ip, port);

  close(coordinator_socket);

  return 0;
}

void command_check(char expected, char got){
  if(expected != got) error("ERROR Invalid Command");
}

void cmd_list_servers(int sock){
  int bytes_recieved, i;
  char send_data[1024], recv_data[1024];

  send(sock, "C", 1, 0);
  recv(sock, recv_data, sizeof(char), 0);

  for(i = 0; i < SERVER_LIST_SIZE; i++){
    recv(sock, &(servers[i]), sizeof(server_info), 0);
    printf("<%d> IP: %d.%d.%d.%d Porta: %d\n", i, servers[i].ip[0], servers[i].ip[1], servers[i].ip[2], servers[i].ip[3], servers[i].port);
  }

  command_check('C', recv_data[0]);

}











