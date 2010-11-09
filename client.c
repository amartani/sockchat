/** * Server for Chat * * Dalton Pinto */#include "common.c"#define COORDINATOR_IP "127.0.0.1"#define COORDINATOR_PORT 5000// Porta: unsigned short (2 bytes)// IP:    unsigned int   (4 bytes)// ------- Function Prototypes --------// void error(char *msg);// string recv_string(int sock);// void send_string(int sock, string str);// void free_string(string str)// ------- Commands --------// void cmd_list_servers(int sock);// void cmd_conect(int sock);// void cmd_send_message(int sock);// void cmd_heartbeat(int sock);// void cmd_list_users(int sock);// void cmd_echo(int sock);// void cmd_unknown(int sock);// void cmd_receive_message(int sock);void sigquit(){ exit(0); }int get_coordinator_socket(){  int sock;  struct sockaddr_in coordinator_address;  struct hostent *host;  sock = socket(AF_INET, SOCK_STREAM, 0);  if(sock < 0) error("ERROR opening coordinator socket");  bzero((char *) &coordinator_address, sizeof(coordinator_address));  coordinator_address.sin_family = AF_INET;  coordinator_address.sin_port = htons(COORDINATOR_PORT);  host = gethostbyname(COORDINATOR_IP);  bcopy((char *)host->h_addr, (char *)&coordinator_address.sin_addr.s_addr, host->h_length);  if ( connect(sock, (struct sockaddr *)&coordinator_address, sizeof(coordinator_address)) < 0 ) error("ERROR connecting");  return sock;}int main(int argc, char *argv[]){  int server_socket, coordinator_socket;  // struct sockaddr_in coordinator_address;  signal(SIGQUIT, sigquit); // Waiting to be killed (kill -QUIT <pid>)  coordinator_socket = get_coordinator_socket();  // coordinator_socket = socket(AF_INET, SOCK_STREAM, 0);  // if(coordinator_socket < 0) error("ERROR opening coordinator socket");  //   // bzero((char *) &coordinator_address, sizeof(coordinator_address));  // coordinator_address.sin_family = AF_INET;  // coordinator_address.sin_port = htons(COORDINATOR_PORT);  // bcopy(COORDINATOR_IP, (char *)&coordinator_address.sin_addr.s_addr, strlen(COORDINATOR_IP));  //   // if ( connect(coordinator_socket, &coordinator_address, sizeof(coordinator_address)) < 0 ) error("ERROR connecting");  close(coordinator_socket);  return 0;}