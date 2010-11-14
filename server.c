/**
 * Server for Chat
 *
 * Alexandre Martani <amartani at gmail com>
 *
 * References:
 * - Threads: http://www.cs.cf.ac.uk/Dave/C/node32.html
 */

//#define DEBUG_THREADS
//#define DEBUG_CMD

#define DEFAULT_PORT 8000
#define HEARTBEAT_TIMEOUT 5

#include "common.c"
#include "thread_helper.c"
#include <netinet/tcp.h>

// function prototypes

// --------- Data structure -----------

// ---- List of clients -----

typedef struct client_node client_node_t;
typedef struct clients_list clients_list_t;

struct client_node {
    int sock;
    string nick;
    client_node_t *prev, *next;
    pthread_t thread;
    pthread_t watchdog;
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
void set_client_socket_options(int sock);
void client_handle(int sock);
void disconnect_user(client_node_t *client_node);
void socket_error_handler();
void set_listening_socket_options(int sockfd);
void set_watchdog(client_node_t *client_node);
void *watchdog(void *arg);
void thread_cancel_if_not_self(pthread_t thread);

// ---- commands ----

void cmd_set_nick(int sock, client_node_t *client_node);
void cmd_list(int sock);
void cmd_echo(int sock);
void cmd_heartbeat(client_node_t *client_node);
void cmd_quit(client_node_t *client_node);
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
    clients_list->lock = new_lock();
}

client_node_t *insert_client(int sock)
{
    client_node_t* new_client = (client_node_t*) malloc(sizeof(client_node_t));
    new_client->sock = sock;
    new_client->nick.size = 0;
    new_client->nick.str = NULL;
    new_client->prev = NULL;
    new_client->next = NULL;

    new_client->thread = pthread_self();
    new_client->watchdog = 0;

    // Lock client list for writer
    wlock(clients_list->lock);

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
    unlock(clients_list->lock);

    return new_client;
}

// main function

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen, res;
    struct sockaddr_in serv_addr, cli_addr;
    pthread_t chld_thr;
    void *arg;

    if (argc < 2) {
        portno = DEFAULT_PORT;
    } else {
        portno = atoi(argv[1]);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    set_listening_socket_options(sockfd);

    bzero((char *) &serv_addr, sizeof(serv_addr));
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
        res = pthread_create(&chld_thr, 0, do_chld, arg);
        if (res)
        {
            error("ERROR creating thread");
        }
    }
    return 0;
}

void set_listening_socket_options(int sockfd)
{
    int optval;

    // Avoid address in use errors
    optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

}

// New threads calls this
void *do_chld(void *arg)
{
    int sock = *((int*)arg);
    free(arg);

    set_client_socket_options(sock);

    client_handle(sock);
}

// Set common options for client sockets
void set_client_socket_options(int sock)
{
    struct timeval tv;

    // Set timeout
//    tv.tv_usec = 100;  /* 100 usec Timeout */
//    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));

}

// Handle a connection with a client

void client_handle(int sock)
{
    client_node_t *client_node = insert_client(sock);

    set_watchdog(client_node);
    while (1) {
        select_command(sock, client_node);
    }
}

// Listen to a command and execute
void select_command(int sock, client_node_t *client_node)
{
    int n;
    char command;

    n = recv(sock, &command, sizeof(command), 0);
    if (n < 1)
    {
        socket_error_handler();
        return;
    }

#ifdef DEBUG_CMD
    printf("Recv command %c\n", command);
    fflush(stdout);
#endif

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
    case 'H':
        cmd_heartbeat(client_node);
        break;
    case 'Q':
        cmd_quit(client_node);
        break;
    default:
        cmd_unknown(sock);
        break;
    }

    return;

}

void socket_error_handler()
{
    int serr = errno;
    switch (errno)
    {
        case EAGAIN:
            break;
        default:
            //perror("Socket error");
            break;
    }
}

// Disconnect user
void disconnect_user(client_node_t *client_node)
{
    client_node_t *next, *prev;

//    printf("Disconnecting %s. -\n", client_node->nick.str);
//    fflush(stdout);

    // Lock client list for writer
    wlock(clients_list->lock);

    // Kill threads
    thread_cancel_if_not_self(client_node->thread);
    thread_cancel_if_not_self(client_node->watchdog);

    // Remove node from global list
    clients_list->size --;

    next = client_node->next;
    prev = client_node->prev;
    if (prev) prev->next = next;
    if (next) next->prev = prev;

    // Unlock client list
    unlock(clients_list->lock);

    // Close socket
    close(client_node->sock);

    // Free memory
    free_string(client_node->nick);
    free(client_node);
}

// commands

void cmd_set_nick(int sock, client_node_t *client_node)
{
    string nick;

    nick = recv_string(sock);

    // Lock client list for writing
    wlock(clients_list->lock);

    free_string(client_node->nick);
    client_node->nick = nick;

    // Unlock
    unlock(clients_list->lock);
}

void cmd_list(int sock)
{
    int i;
    client_node_t *client_node;

    // Send command
    send_forced(sock, "L", 1);

    // Lock client list for reading
    rlock(clients_list->lock);

    // Send number of clients
    send_forced(sock, &clients_list->size, sizeof(clients_list->size));

    // Send nickname of each client
    for (client_node = clients_list->first; client_node != NULL; client_node = client_node->next) {
        send_string(sock, client_node->nick);
    }

    // Unlock
    unlock(clients_list->lock);
}

void cmd_echo(int sock)
{
    string str;
    int res;

    str = recv_string(sock);

    send_forced(sock, "E", 1);

    send_string(sock, str);

    free_string(str);
}

void cmd_heartbeat(client_node_t *client_node)
{
    set_watchdog(client_node);
}

void cmd_quit(client_node_t *client_node)
{
    //thread_cancel_if_not_self(client_node->watchdog);
    //thread_cancel_if_not_self(client_node->thread);
    disconnect_user(client_node);
    pthread_exit(NULL);
}

void cmd_unknown(int sock)
{
    int n;
    string msg = string_create("Desconhecido");

    send_forced(sock, "E", 1);
    send_string(sock, msg);
}

void set_watchdog(client_node_t *client_node)
{
    if (client_node->watchdog) {
        pthread_cancel(client_node->watchdog);
    }

    pthread_create(&client_node->watchdog, NULL, watchdog, (void*) client_node);
}

void *watchdog(void *arg)
{
    client_node_t* client_node = (client_node_t*) arg;

    sleep(HEARTBEAT_TIMEOUT);

    disconnect_user(arg);
}

void thread_cancel_if_not_self(pthread_t thread)
{
    if (thread == 0) return;
    if (pthread_equal(thread, pthread_self()) == 0) return;
    pthread_cancel(thread);
}

