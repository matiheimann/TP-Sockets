#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>  
#include <arpa/inet.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/un.h>
#include <sys/time.h>
#include <pthread.h>
#include "server.h"
#include "database.h"

int main(int argc, char const *argv[])
{
	int serverDescriptor;
	struct sockaddr_un serverAddress;
	serverDescriptor = socket(AF_UNIX, SOCK_STREAM, 0);
	if(serverDescriptor == -1)
	{
		perror("Socket could not be created");
		exit(1);
	}
	memset(&serverAddress, 0, sizeof(struct sockaddr_un));
	serverAddress.sun_family = AF_UNIX;
	strncpy(serverAddress.sun_path, SOCKET_PATH, sizeof(serverAddress.sun_path));
	if(connect(serverDescriptor, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) ==-1)
	{
		perror("Bind error");
		exit(1);
	}
	while(1)
	{
		char* buffer = calloc(200, 1);
		/*habria que cambiar el mensaje que se manda por los comandos
		leidos con scanf que pide la consigna*/
		if(write(serverDescriptor, "Hi im client", 12) == -1)
		{
			perror("write error");
			exit(1);
		}
		if(read(serverDescriptor, buffer, 13) == -1)
		{
			perror("read error");
			exit(1);
		}
		printf("%s\n", buffer);
		sleep(1);
		free(buffer);
	}
	close(serverDescriptor);

	return 0;
}