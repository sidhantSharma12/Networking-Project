#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAX_LISTEN_BACKLOG 1
#define BUFFER_SIZE 4096

                             //client socket connection
                             //address of backend connecting to
                             //port of the backend address
void handle_client_connection(int clientSocketFd, char *backendHost, char *portNumberBackend) {

    struct addrinfo hints; //used to give hints
    struct addrinfo *possibleAddresses; //contains the addresses we can connect to
    struct addrinfo *possibleAddressesIter;
    int getaddrinfo_error;

    int backendSocketFd;

    char buffer[BUFFER_SIZE];
    int bytesRead;

    memset(&hints, 0, sizeof(struct addrinfo));//clear hints since its defined on the stack
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;//hint is that we want something that supports streaming sockets

    getaddrinfo_error = getaddrinfo(backendHost, portNumberBackend, &hints, &possibleAddresses);//returns zero if successful. Address info is returned in possibleAddresses
    if (getaddrinfo_error != 0) {
        fprintf(stderr, "Couldn't find backend: %s\n", gai_strerror(getaddrinfo_error));
        exit(1);
    }

    for (possibleAddressesIter = possibleAddresses;  possibleAddressesIter != NULL;  possibleAddressesIter = possibleAddressesIter->ai_next) {
        //try creating a socket with each possible address. 
        backendSocketFd = socket(possibleAddressesIter->ai_family, possibleAddressesIter->ai_socktype, possibleAddressesIter->ai_protocol);
        
        if (backendSocketFd == -1) {
            continue;
        }
        //if succeeded, we connect to the address using that socket
        if (connect(backendSocketFd, possibleAddressesIter->ai_addr, possibleAddressesIter->ai_addrlen) != -1) { 
            break;
        }
        //if connection failed, try again
        close(backendSocketFd);
    }

    //check to see if we ever managed to create a successful connection
    if (possibleAddressesIter == NULL) {
        fprintf(stderr, "Couldn't connect to backend");
        exit(1);
    }

    //free all addresses. Memory management!
    freeaddrinfo(possibleAddresses);

    //read from client and send to backend
    bytesRead = read(clientSocketFd, buffer, BUFFER_SIZE);
    write(backendSocketFd, buffer, bytesRead);

    //read everything from the backend and write it to client until read returns zero bytes
    while (bytesRead = read(backendSocketFd, buffer, BUFFER_SIZE)) {
        write(clientSocketFd, buffer, bytesRead);
    }

    close(clientSocketFd);
}


int main(int argc, char *argv[]) {
    char *server_port_str;
    char *backendAddr;
    char *portNumberBackend;

    struct addrinfo hints;//same hints as function
    struct addrinfo *possibleAddresses;
    struct addrinfo *addrIter;
    int getaddrinfo_error;

    int serverSocketFd;
    int clientSocketFd;

    int so_reuseaddr;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server_port> <backendAddr> <backend_port>\n", argv[0]);
        exit(1);
    }
    server_port_str = argv[1];
    backendAddr = argv[2];
    portNumberBackend = argv[3];

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    //AI_PASSIVE, combined with the NULL first parameter, tells getaddrinfo that we want to be able to run a server socket on this address
    getaddrinfo_error = getaddrinfo(NULL, server_port_str, &hints, &possibleAddresses);

    if (getaddrinfo_error != 0) {
        fprintf(stderr, "Couldn't find local host details: %s\n", gai_strerror(getaddrinfo_error));
        exit(1);
    }

    //try to bind so that we can accept incoming connections:
    for (addrIter = possibleAddresses; addrIter != NULL; addrIter = addrIter->ai_next) {
        serverSocketFd = socket(addrIter->ai_family, addrIter->ai_socktype, addrIter->ai_protocol);
        if (serverSocketFd == -1) {
            continue;
        }

        so_reuseaddr = 1;
        setsockopt(serverSocketFd, SOL_SOCKET, SO_REUSEADDR, &so_reuseaddr, sizeof(so_reuseaddr));

        if (bind(serverSocketFd, addrIter->ai_addr, addrIter->ai_addrlen) == 0){
            break;
        }

        close(serverSocketFd);
    }

    if (addrIter == NULL) {
        fprintf(stderr, "Couldn't bind\n");
        exit(1);
    }

    freeaddrinfo(possibleAddresses);

    listen(serverSocketFd, MAX_LISTEN_BACKLOG);
    printf("Started.  Listening on port %s.\n", server_port_str);

    while (1) {
        clientSocketFd = accept(serverSocketFd, NULL, NULL);
        if (clientSocketFd == -1) {
            perror("Could not accept");
            exit(1);
        }

        handle_client_connection(clientSocketFd, backendAddr, portNumberBackend);
    }

}