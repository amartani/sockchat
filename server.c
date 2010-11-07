/**
 * Server for Chat
 *
 * Alexandre Martani <amartani at gmail com>
 *
 * References:
 * - Threads: http://www.cs.cf.ac.uk/Dave/C/node32.html
 */

#include "common.c"

// function prototypes

// --------- Data structure -----------

// ---- List of clients -----

typedef struct client_node client_node_t;
typedef struct clients_list clients_list_t;

struct client_node {
    int sock;
    string nick;
    client_node_t *prev, *next;
};

struct clients_list {
    pthread_rwlock_t *lock;
    client_node_t *first, *last;
    int size;
};

void initialize_clients_list();
client_node_t *insert_client(int sock);

// ---- Helper functions ----

string recv_string(int sock);
void send_string(int sock, string str);

void *do_chld(void *arg);
void select_command(int sock, client_node_t *client_node);
void client_handle(int sock);

// ---- commands ----

void cmd_set_nick(int sock, client_node_t *client_node);
void cmd_list(int sock);
void cmd_echo(int sock);
void cmd_unknown(int sock);

// -------- Functions -----------

// ---- List of clients  ----

clients_list_t *clients_list;

void initialize_clients_list()
{
    clients_list = (clients_list_t*)malloc(sizeof(clients_list_t));

    clients_list->first = NULL;
    clients_list->last = NULL;
    clients_list->size = 0;

    // Initialize lock
    clients_list->lock = (pthread_rwlock_t*) malloc(sizeof(pthread_rwlock_t));
    pthread_rwlock_init(clients_list->lock, NULL);
}

client_node_t *insert_client(int sock)
{
    client_node_t* new_client = (client_node_t*) malloc(sizeof(client_node_t));
    new_client->sock = sock;
    new_client->nick.size = 0;
    new_client->nick.str = NULL;
    new_client->prev = NULL;
    new_client->next = NULL;

    // Lock client list for writer
    pthread_rwlock_wrlock(clients_list->lock);

    clients_list->size++;

    if (clients_list->first == NULL) {
        clients_list->first = new_client;
        clients_list->last = new_client;
    } else {
        new_client->prev = clients_list->last;
        clients_list->last->next = new_client;
        clients_list->last = new_client;
    }

    // Unlock client list
    pthread_rwlock_unlock(clients_list->lock);

    return new_client;
}

// main function

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    pthread_t chld_thr;
    void *arg;

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

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

    initialize_clients_list();

    // Loop forever accepting connections
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
        pthread_create(&chld_thr, 0, do_chld, arg);
    }
    return 0;
}

// New threads calls this
void *do_chld(void *arg)
{
    int sock = *((int*)arg);
    free(arg);

    client_handle(sock);
}

// Handle a connection with a client

void client_handle(int sock)
{
    client_node_t *client_node = insert_client(sock);

    while (1) {
        select_command(sock, client_node);
    }
}

// Listen to a command and execute
void select_command(int sock, client_node_t *client_node)
{
    int n;
    char command;

    n = read(sock, &command, sizeof(command));
    if (n < 0) error("ERROR reading from socket");

    switch (command) {
    case 'C':
        cmd_set_nick(sock, client_node);
        break;
    case 'L':
        cmd_list(sock);
        break;
    case 'E':
        cmd_echo(sock);
        break;
    default:
        cmd_unknown(sock);
        break;
    }
}

// commands

void cmd_set_nick(int sock, client_node_t *client_node)
{
    string nick;

    nick = recv_string(sock);

    // Lock client list for writing
    pthread_rwlock_wrlock(clients_list->lock);

    free_string(client_node->nick);
    client_node->nick = nick;

    // Unlock
    pthread_rwlock_unlock(clients_list->lock);
}

void cmd_list(int sock)
{
    int res, i;
    client_node_t *client_node;

    // Send command
    res = write(sock, "L", 1);
    if (res < 0) error("ERROR writing to socket");

    // Lock client list for reading
    pthread_rwlock_rdlock(clients_list->lock);

    // Send number of clients
    res = write(sock, &clients_list->size, sizeof(clients_list->size));
    if (res < 0) error("ERROR writing to socket");

    // Send nickname of each client
    for (client_node = clients_list->first; client_node != NULL; client_node = client_node->next) {
        send_string(sock, client_node->nick);
    }

    // Unlock
    pthread_rwlock_unlock(clients_list->lock);
}

void cmd_echo(int sock)
{
    string str;
    int res;

    str = recv_string(sock);

    res = write(sock, "E", 1);
    if (res < 0) error("ERROR writing to socket");

    send_string(sock, str);
}

void cmd_unknown(int sock)
{
    int n;

    n = write(sock,"Desconhecido",12);
    if (n < 0) error("ERROR writing to socket");
}

