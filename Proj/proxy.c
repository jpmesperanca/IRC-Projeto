/*******************************************************************************
 * SERVIDOR no porto 9000, à escuta de novos clientes.  Quando surjem
 * novos clientes os dados por eles enviados são lidos e descarregados no ecran.
 *******************************************************************************/
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SERVER_PORT     9001
#define BUF_SIZE	1024

void process_client(int fd);
void erro(char *msg);
void readingFiles(int client_fd, int server_fd);
int checkCommand(char* buffer, int client_fd, int server_fd);
void transferFile(int client_fd, int server_fd);
void connecta(int client_fd);
void work(int client_fd, int server_fd);

int main() {
	int fd, client;
	struct sockaddr_in addr, client_addr;
	int client_addr_size;

	bzero((void *) &addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(SERVER_PORT);

	if ( (fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		erro("na funcao socket");
	if ( bind(fd,(struct sockaddr*)&addr,sizeof(addr)) < 0)
		erro("na funcao bind");
	if( listen(fd, 5) < 0)
		erro("na funcao listen");
	client_addr_size = sizeof(client_addr);
	while (1) {
		//clean finished child processes, avoiding zombies
		//must use WNOHANG or would block whenever a child process was working
		while(waitpid(-1,NULL,WNOHANG)>0);
		//wait for new connection
		client = accept(fd,(struct sockaddr *)&client_addr,(socklen_t *)&client_addr_size);
		if (client > 0) {
			if (fork() == 0) {
				close(fd);
				connecta(client);
				exit(0);
			}
		close(client);
		}
	}
	return 0;
}

void connecta(int client_fd)
{

	int nread = 0;
	char buffer[BUF_SIZE];
	int fd;
	struct sockaddr_in addr;
	struct hostent *hostPtr;

	nread = read(client_fd, buffer, BUF_SIZE-1);
	buffer[nread] = '\0';
	printf("%s\n", buffer);

	if ((hostPtr = gethostbyname(buffer)) == 0)
		erro("Nao consegui obter endereço");	

	bzero((void *) &addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ((struct in_addr *)(hostPtr->h_addr))->s_addr;
	addr.sin_port = htons((short) atoi("9000"));

	if((fd = socket(AF_INET,SOCK_STREAM,0)) == -1)
		erro("socket");
	if( connect(fd,(struct sockaddr *)&addr,sizeof (addr)) < 0)
		erro("Connect");
	
	work(client_fd, fd);

	close(client_fd);
}

void work(int client_fd, int server_fd){

	int nread;
	char buffer[BUF_SIZE];

	while(1){

		memset(buffer, 0, sizeof(buffer));

		nread = read(client_fd, buffer, BUF_SIZE-1);
		buffer[nread] = '\0';
		puts(buffer);

		if (strcmp(buffer, "QUIT") == 0){
			write(server_fd, buffer, strlen(buffer));
			printf("Quitting\n");
			break; 
		}
		/*
		else if (strcmp(buffer, "LIST") == 0){
			write(server_fd, buffer, strlen(buffer));
			readingFiles(client_fd, server_fd);
		}*/

		else{
			if (checkCommand(buffer,client_fd,server_fd)){
				write(client_fd, "0", 2);
				transferFile(client_fd,server_fd);
			}
			else
				write(client_fd, "1", 2);
		}
	}

	close(client_fd);
	close(server_fd);
}

void erro(char *msg){

	printf("Erro: %s\n", msg);
	exit(-1);
}

void readingFiles(int client_fd, int server_fd){


}

int checkCommand(char* buffer, int client_fd, int server_fd){

	char command[100], TCPUDP[100], encryption[100] , file[100], aux[200];
	
	if (sscanf(buffer, "%s %s %s %s", command, TCPUDP, encryption, file) == EOF){
		write(client_fd, "1", 2);	
		return 1;
	}
	
	if (strcmp(command, "DOWNLOAD") != 0 || (strcmp(TCPUDP, "TCP") != 0 && strcmp(TCPUDP, "UDP") != 0 ) || (strcmp(encryption, "ENC") != 0 && strcmp(encryption, "NOR") != 0)){
		write(client_fd, "1", 2);	
		return 1;
	} 

	if (strcmp(encryption, "ENC") == 0)
		sprintf(aux, "%s,1", file);

	write(server_fd, aux, strlen(aux));
	printf("Enviei aux\n");
	write(client_fd, file, strlen(file));
	printf("Enviei ficheiro\n");

	return 0;
}

void transferFile(int client_fd, int server_fd){

	char buffer[BUF_SIZE];
	int nread;

	while (1){

		nread = read(server_fd, buffer, BUF_SIZE);
		write(client_fd,buffer,strlen(buffer)); 

		if (nread < BUF_SIZE)
			break;	
	}

	printf("End of transfer\n");
}