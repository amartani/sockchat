/**
 * Server for Chat
 *
 * Dalton Pinto
 */

#include "common.c"

// Porta: unsigned short (2 bytes)
// IP:    unsigned int   (4 bytes)

// ------- Function Prototypes --------
// void error(char *msg);
// string recv_string(int sock);
// void send_string(int sock, string str);
// void free_string(string str)

// ------- Commands --------
// void cmd_list_servers(int sock);
// void cmd_conect(int sock);
// void cmd_send_message(int sock);
// void cmd_heartbeat(int sock);
// void cmd_list_users(int sock);
// void cmd_echo(int sock);
// void cmd_unknown(int sock);
// void cmd_receive_message(int sock);

void sigquit(){ exit(0); }

int main(int argc, char *argv[])
{
  signal(SIGQUIT, sigquit); // Waiting to be killed (kill -QUIT <pid>)

  return 0;
}