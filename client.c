/**
 * Server for Chat
 *
 * Dalton Pinto
 */

#include "common.c"

// Porta: unsigned short (2 bytes)
// IP:    unsigned int   (4 bytes)

// ------- Function Prototypes --------
void error(char *msg);
string recv_string(int sock);
void send_string(int sock, string str);
void free_string(string str)

// ------- Commands --------
void cmd_list_servers(int sock);
void cmd_conect(int sock);
void cmd_send_message(int sock);
void cmd_heartbeat(int sock);
void cmd_list_users(int sock);
void cmd_echo(int sock);
void cmd_unknown(int sock);
void cmd_receive_message(int sock);

int main(int argc, char *argv[])
{
  int sockfd, newsockfd, portno, clilen;
  struct sockaddr_in serv_addr, cli_addr;
  pthread_t chld_thr;
  void *arg;

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
}