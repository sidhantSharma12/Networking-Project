#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(char *msg){
	perror(msg);
	exit(0);
}
//first you create a socket. Then you connect to the server, and then write and read from the server. 
//This is different from server side where you bind and listen for connections. In client, the socket chooses
//where its going to remotely connect to.
int main(int argc, char *argv[]){
	int sockfd, portno, n;
	struct sockaddr_in server_address;
	struct hostent* server;

	char buffer[256];

	if (argc < 2){
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
	}

	portno = atoi(argv[1]);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd<0){
		error("There was an error opening the socket");
	}

	bzero((char *) &server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;

	server_address.sin_addr.s_addr= INADDR_ANY;

	server_address.sin_port = htons(portno);

	if (connect(sockfd,&server_address,sizeof(server_address)) < 0){
		error("ERROR connecting");
	}

	printf("Please enter the message: ");
	bzero(buffer,256);
	fgets(buffer,255,stdin);
	n = write(sockfd,buffer,strlen(buffer));

	if (n < 0){
		error("ERROR writing to socket");
	}

	bzero(buffer,256);
	n = read(sockfd,buffer,255);

	if (n < 0){
		error("ERROR reading from socket");
	}

	printf("%s\n",buffer);
	return 0;
}

