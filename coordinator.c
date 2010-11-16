/**
 * Coordinator for Chat
 *
 * Vinicius Battagello <battagello at gmail com>
 *
 * References:
 * - Threads: http://www.cs.cf.ac.uk/Dave/C/node32.html
 */

// TODO geral: ver se as implementacoes de send_server_info e recv_server_info em common.c estao corretas

#include "common.c"
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

// constants



// --------- Data structure -----------
struct server_node {
    int sock;
    unsigned int ip[4];
    int port;
    time_t last_seen;
};

typedef struct server_node server_node;

// global_vars
server_node server_list[SERVERS];
char* server_exec;

// function prototypes

// converte um server_node para server_info
server_info to_server_info(server_node s);


void initialize_server_list();
void print_server_list();
void print_server_node(server_node s);

void start_servers();
void start_server(char*, int server_port);
void *do_heartbeat(void *);
int get_socket(char *, int port);

void start_monitor_thread();
void *monitor_thread_loop(void *);

// inicializa a parte "servidora" do coordinator
void handle_connections();

// metodo da thread de handle connections
void *handle_connection(void *);
void handle_command(int sock);

// ---- commands ----

void cmd_s(int sock);
void cmd_c(int sock);

// inicializa a thread que monitora se os servidores estao online

// inicializa um servidor (server exec é o comando, server port é a porta)
void start_server(char *server_exec, int server_port) {
    char comando[1024] = "";
    char porta[1024];
    char *argv[3];

    sprintf(porta, "%d", server_port);
    strcat(comando, server_exec);
    strcat(comando, " ");
    strcat(comando, porta);
    printf("Inicializando servidor. executando comando: [%s]\n", comando);
    // TODO resolver como starta os servidores...
    // dica: alexandre disse que dah pra usar execve e fork, mas eu nao sei fazer

    argv[0] = server_exec;
    argv[1] = porta;
    argv[2] = NULL;
    if (fork() == 0) {
        execv(server_exec, argv);
    }

    // Copy argument to be passed to do_heartbeat
    void *arg;
    arg = malloc(sizeof(server_port));
    memcpy(arg, &server_port, sizeof(server_port));

    // Create new thread
    pthread_t dhb_thr;
    int res = pthread_create(&dhb_thr, 0, do_heartbeat, arg);
    if (res)
    {
            error("ERROR creating thread");
    } 
}

// New threads calls this
void *do_heartbeat(void *arg)
{
    int boo = 0; 
   
    // create the server
    int server_port = *((int*)arg);	
    server_node sn;
    sn.port = server_port;

    char letter_send = 'S';
    printf("Chegou ANTES do get_socket\n");
    sleep(5);
    int sock = get_socket("127.0.0.1",server_port);
    printf("Chegou DEPOIS do get_socket\n");
    free(arg);    

	
    while (1) {



	//send "S" to server
	printf("Chegou ANTES do send\n");
	send_forced(sock, &letter_send, sizeof(letter_send));
	printf("Chegou DEPOIS do send\n");

	//try to listen the server	
	struct timeval tv;

    	// Set timeout
    	tv.tv_sec = 5;
    	tv.tv_usec = 0;  /* 5 sec Timeout */
    	if(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval))==0)
	{
	    printf("Chegou ANTES do receive\n");
	    char c;
            int i;
	    i = recv(sock, &c, sizeof(char), 0);
	    printf("Chegou DEPOIS do receive\n");
	    if ((i==1)&&(c=='S')) {	}
  	    else 
	    { 
		printf("ERRO NO RECEIVE\n");
		// to do_heartbeat again
 		do_heartbeat(&arg);
	    } 	
	}
   }
   printf("Enviou e recebeu do servidor o heartbeat 'S'\n"); 
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


// -------- Functions -----------

// ---- List of clients  ----server_list[i].ip[0]

void initialize_server_list()
{
    int i;
    for(i = 0; i < SERVERS; i++) {
        server_list[i].port = i + SERVER_PORT_START;
        server_list[i].ip[0] = 127;
        server_list[i].ip[1] = 0;
        server_list[i].ip[2] = 0;
        server_list[i].ip[3] = 1;
        server_list[i].last_seen = time(NULL);
    }
}

void print_server_list() {
    int i;
    for(i = 0; i < SERVERS; i++) {
        print_server_node(server_list[i]);
    }
}

void print_server_info(server_info s) {
    struct tm *ts;
    char buf[80];
    printf("Server_info.\n");
    printf("IP: %d.%d.%d.%d\n", s.ip[0], s.ip[1], s.ip[2], s.ip[3]);
    printf("Porta: %d\n", s.port);
}

void print_server_node(server_node s) {
    struct tm *ts;
    char buf[80];
    printf("Noh servidor.\n");
    printf("IP: %d.%d.%d.%d\n", s.ip[0], s.ip[1], s.ip[2], s.ip[3]);
    printf("Porta: %d\n", s.port);
    /* Format and print the time, "ddd yyyy-mm-dd hh:mm:ss zzz" */
    ts = localtime(&(s.last_seen));
    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
    printf("Visto pela ultima vez: %s\n\n", buf);
}

void start_servers() {
    int i = 0;
    for(i = 0; i < SERVERS; i++) {
        start_server(server_exec, server_list[i].port);
    }
}

void start_monitor_thread() {
    pthread_t monitor_thread;
    // criando thread sem argumento
    pthread_create(&monitor_thread, 0, monitor_thread_loop, (void *)NULL);
}

void* monitor_thread_loop(void* arg) {
    int i = 0;
    printf("Entrando na Thread monitora\n");
    while(1) {
        // TODO 1) loop na lista de servidores
        // 2) Se algum servidor morreu, restarte-o com start_server
        // Desafio: vai ter race condition na lista -> talvez seja bom usar o lock.
        printf("Monitor Thread Loop %d\n", i);
        sleep(1);
        i++;
    }
}

void handle_connections() {
    int sockfd, newsockfd, portno, clilen;
    void *arg;
    struct sockaddr_in serv_addr, cli_addr;
    pthread_t chld_thr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = COORDINATOR_PORT;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
            sizeof(serv_addr)) < 0)
            error("ERROR on binding");
    listen(sockfd, 5);

    // Loop forever accepting connections
    // for each connection accepted, start thread and handle command
    while (1) {
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd,
                           (struct sockaddr *) &cli_addr,
                           &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");

        // Copy argument to be passed to do_chld
        arg = malloc(sizeof(newsockfd));
        memcpy(arg, &newsockfd, sizeof(newsockfd));

        // Create new thread
        pthread_create(&chld_thr, 0, handle_connection, arg);
    }
}

// pega o socket e passa para handle_command
void *handle_connection(void *arg)
{
    int sock = *((int*)arg);
    free(arg);

    handle_command(sock);
}


void handle_command(int sock)
{
    int n;
    char command;

    while (1) {
        n = read(sock, &command, sizeof(command));
        if(n < 0) {
            // conexao caiu!!! fazer o seguinte:
            // 1) fechar conexao
	    close(sock); 

            // 2) logar erro
            error("Conexao caiu!");
            // 3) sair da thread
            pthread_exit(0);
            break; // inutil, pois a thread ja vai ter saido
        }

        switch (command) {
          case 'C':
            cmd_c(sock);
            break;
          case 'S':
            cmd_s(sock);
            break;
          default:
            printf("Coordenador recebeu comando desconhecido");
            break;
        }    
    }
}

void cmd_s(int sock) {
    //TODO descobrir como vou saber qual servidor me mandou o comando S (talvez o servidor devesse enviar a porta

    //TODO atualizar o last_seen do servidor que mandou a mensagem
}

// manda informacoes de todos os servidores
// fecha a conexao
// sai da thread
void cmd_c(int sock) {
   pthread_t this_thread;
   server_info s;
   int i;

   send_forced(sock, "C", 1);

   for(i = 0; i < SERVERS; i++) {
        s = to_server_info(server_list[i]);
        send(sock, &s, sizeof(server_info), 0);
        printf("Enviando informacao de servidor: \n");
        print_server_info(s);
    }
    close(sock);
    this_thread = pthread_self();
    pthread_exit(&this_thread);
}

// main function
// argv deve ter o nome do executavel do servidor
int main(int argc, char *argv[])
{
    pthread_t main_thread = pthread_self();
    if(argc < 2) {
       error("eh preciso informar o nome do executavel do servidor");
    }
    server_exec = argv[1];
    initialize_server_list();
    start_servers();
    print_server_list();
    //start_monitor_thread();
    
    handle_connections();
    pthread_exit(&main_thread);
}

server_info to_server_info(server_node s) {
    int i;
    server_info result;
    for(i = 0; i < 4; i++) {
       result.ip[i] = s.ip[i];
    }
    result.port = s.port;
    return result;
}


