/**
 * Server for Chat
 *
 * Dalton Pinto
 */

#include "common.c"

#define COORDINATOR_IP "127.0.0.1"
#define COORDINATOR_PORT 5000

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
  printf("(C) IP: %u.%u.%u.%u Porta: %u\n", (unsigned int) *(host->h_addr), (unsigned int) *(host->h_addr+1), (unsigned int) *(host->h_addr+2), (unsigned int) *(host->h_addr+3), (unsigned int) coordinator_address.sin_port);
  printf("(C) IP: %u Porta: %u\n", (unsigned int) *(host->h_addr), (unsigned int) coordinator_address.sin_port);
  bzero(&(coordinator_address.sin_zero),8);
  if( connect(sock, (struct sockaddr *)&coordinator_address, sizeof(coordinator_address)) < 0 ) error("ERROR connecting");
  return sock;
}

void choose_server(char *string_ip, int *port){
  char code;
  scanf("%c", &code);
  if(code != 'C') error("ERROR Needs to type C for choose servers!");
  scanf("%s %d", string_ip, port);
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

  // choose_server(string_ip, &port);
  printf("IP: %s Porta: %d\n", string_ip, port);

  close(coordinator_socket);

  return 0;
}

void command_check(char expected, char got){
  if(expected != got) error("ERROR Invalid Command");
}

void cmd_list_servers(int sock){
  int bytes_recieved;
  server_info servers[10];
  char send_data[1024], recv_data[1024];

  send(sock, "C", 1, 0);
  recv(sock, recv_data, sizeof(char), 0);
  bytes_recieved = recv(sock, servers, sizeof(server_info)*10, 0);
  printf("(Ruby) IP: %d.%d.%d.%d Porta: %u\n", servers[0].ip[0], servers[0].ip[1], servers[0].ip[2], servers[0].ip[3], servers[0].port);
  // recv_data[bytes_recieved] = '\0';

  command_check('C', recv_data[0]);

}











