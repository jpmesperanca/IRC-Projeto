/**********************************************************************
 * CLIENTE liga ao servidor (definido em argv[1]) no porto especificado  
 * (em argv[2]), escrevendo a palavra predefinida (em argv[3]).
 * USO: >cliente <enderecoServidor>  <porto>  <Palavra>
 **********************************************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#define BUF_SIZE	10240

void erro(char *msg);
void process_server(int server_fd);
void readingFiles(int server_fd);
void receiveTCP(int server_fd);

int main(int argc, char *argv[]) {

	char endServer[100];
	int fd;
	struct sockaddr_in addr;
	struct hostent *hostPtr;

	if (argc != 3) {
		printf("cliente <host> <port> \n");
		exit(-1);
	}

	strcpy(endServer, argv[1]);

	if ((hostPtr = gethostbyname(endServer)) == 0)
		erro("Nao consegui obter endereço");	

	bzero((void *) &addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
	addr.sin_port = htons((short) atoi(argv[2]));

	if((fd = socket(AF_INET,SOCK_STREAM,0)) == -1)
		erro("socket");
	if( connect(fd,(struct sockaddr *)&addr,sizeof (addr)) < 0)
		erro("Connect");

	process_server(fd);

	close(fd);
	exit(0);
}

void erro(char *msg) {
	printf("Erro: %s\n", msg);
	exit(-1);
}

void process_server(int server_fd){

	char buffer[BUF_SIZE];
	int nread;

	while(1){

		memset(buffer, 0, sizeof(buffer));
		
		fgets(buffer, BUF_SIZE-1, stdin);
		write(server_fd, buffer, strlen(buffer));

		if (strcmp(buffer, "QUIT") == 0)
			break;

		if (strcmp(buffer, "LIST") == 0){
			printf("\nListing:\n");
			readingFiles(server_fd);
			printf("-----\n\n");
		}

		else{
			memset(buffer, 0, sizeof(buffer));

			nread = read(server_fd, buffer, BUF_SIZE-1);
			buffer[nread] = '\0';

			if (strcmp(buffer,"0") == 0){
				write(server_fd, "check", 6);
				receiveTCP(server_fd);
			}
		}
	}

	close(server_fd);
	exit(0);
}

void readingFiles(int server_fd){

	char buffer[BUF_SIZE];
	int nread;

	while (1){

		memset(buffer, 0, sizeof(buffer));
		nread = read(server_fd, buffer, BUF_SIZE-1);
		buffer[nread] = '\0';
		
		if (strcmp(buffer, "end") == 0)
			break;

		printf("%s\n", buffer);

		write(server_fd, "check", 6);

		fflush(stdout);
		fflush(stdin);
	}
}

void receiveTCP(int server_fd){

	char buffer[BUF_SIZE];
	char dir[2*BUF_SIZE];
	int nread;
	FILE* f;

	memset(buffer, 0, sizeof(buffer));
	nread = read(server_fd, buffer, BUF_SIZE-1);
	buffer[nread] = '\0';
	puts(buffer);

	sprintf(dir, "./Downloads/%s", buffer);

	f = fopen(dir, "wb");

	write(server_fd, "check", 6);

	while (1){

		memset(buffer, 0, sizeof(buffer));
		nread = read(server_fd, buffer, BUF_SIZE-1);
		//buffer[nread] = '\0';

		if (strcmp(buffer, "end") == 0)
			break;

		else{

			fwrite(buffer,strlen(buffer),1,f); 
			write(server_fd, "check", 6);
		}
		
		fflush(stdout);
		fflush(stdin);
	}

	fclose(f);
	printf("End of transfer\n");
}