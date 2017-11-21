#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>

int main(){
	//returning a sample file from the server
	struct sockaddr_in serverAddress;
	FILE* txt_data;
	txt_data = fopen("exampleFile.txt", "r");

	char responseData[512];
	fgets(responseData, 512, txt_data);

	char httpData[1024] = "HTTP/1.1 200 OK\r\n\n";
	strcat(httpData, responseData);

	int serverSocket;
	serverSocket= socket(AF_INET, SOCK_STREAM, 0);

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr= INADDR_ANY;
	serverAddress.sin_port = htons(8080);

	bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
	listen(serverSocket, 5);
	while(1){
		int clientSocket= accept(serverSocket, NULL, NULL);
		send(clientSocket, httpData, sizeof(httpData), 0);
		close(clientSocket);
	} 

	return 0; 
}