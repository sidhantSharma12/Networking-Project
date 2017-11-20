// Server using TCP, with port number passed in as an argument.

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


void error(char* message){
	perror(message);
	exit(1);
}
//first you create a socket. Then you bind that socket to an ip address and port
//number where it can listen to connections. Then you accept a connection and then
//you send or recieve data to other sockets it has connected to.
int main(int argc, char *argv[]){

	int sockfd, newsockfd, portno, clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	if (argc < 2){
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);// AF_INET: using concept of ip address. SOCK_STREAM: Using tcp

	if (sockfd < 0){
		error("ERROR opening socket");
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));//clearing the structure

	portno = atoi(argv[1]);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;//server_addr will get the ip address automatically when started
	serv_addr.sin_port = htons(portno);//converting to network format

	//serv_addr variable has all the info now. Now bind the socket to the server address
	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
		error("ERROR on binding");
	}

	listen(sockfd,5);//5 is how many clients you can handle
	clilen = sizeof(cli_addr);

	newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);//client address will now have all the values

	if (newsockfd < 0){
		error("ERROR on accept");
	}

	bzero(buffer,256);//clear buffer

	n = read(newsockfd,buffer,255);//read into buffer and size of it is 255

	if (n < 0){
		error("ERROR reading from socket");
	}

	printf("Here is the message: %s\n",buffer);

	n = write(newsockfd,"I got your message",18);//write to newsockfd(it has the client info)

	if (n < 0){
		error("ERROR writing to socket");
	}

	return 0;
}





