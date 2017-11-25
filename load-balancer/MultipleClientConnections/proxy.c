#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>

#include "epollinterface.h"
#include "server_socket.h"

int main(int argc, char* argv[]){
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server_port> <backendDomain> <backend_port>\n",  argv[0]);
        exit(1);
    }
    char* proxyPort = argv[1];
    char* backendDomain = argv[2];
    char* backendPort = argv[3];
    
    //epoll allows you to wait for stuff to happen on multiple file descriptors at a time
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("Couldn't create epoll FD");
        exit(1);
    }
    //need to register some file descriptors with it to listen to
    //first one is the one that will wait for incoming connections from clients
    struct epoll_event_handler* serverSocketEventHandler;
    serverSocketEventHandler = create_server_socket_handler(epoll_fd, proxyPort, backendDomain, backendPort);

    //callback function when an event occurs on an fd
    //EPOLLIN: event mask that says that we’re interested in hearing from it when there’s something to read from (client connection has come in)
    add_epoll_handler(epoll_fd, serverSocketEventHandler, EPOLLIN);

    printf("Listening on port %s.\n", proxyPort);
    do_reactor_loop(epoll_fd);

    return 0;
}