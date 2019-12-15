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
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>

#define SERVER_PORT     9000
#define BUF_SIZE	1024

void process_client(int fd);
void erro(char *msg);
void listingFiles(int client_fd);
//int processDownload(int client_fd, char* buffer);	
void trasnferTCP(int client_fd, char* buf);

int protocolo = 0; // 0-> TCP, 1-> UDP

int main() {
	int fd, client;
	struct sockaddr_in addr, client_addr;
	int client_addr_size;

	/*//UDP 
	int s;
	struct sockaddr_in si_minha, si_outra;

	if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		erro("Erro na criação do socket");
	}

	si_minha.sin_family = AF_INET;
	si_minha.sin_port = htons(SERVER_PORT);
	si_minha.sin_addr.s_addr = htonl(INADDR_ANY);

	// Associa o socket à informação de endereço
	if(bind(s,(struct sockaddr*)&si_minha, sizeof(si_minha)) == -1)
	{
		erro("Erro no bind");
	}

	// END OF UDP*/

	bzero((void *) &addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.2");
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
				process_client(client);
				exit(0);
			}
		close(client);
		}
	}
	return 0;
}

void process_client(int client_fd)
{
	int nread;
	char buffer[BUF_SIZE];

	while(1){

		memset(buffer, 0, sizeof(buffer));
		printf("READY TO READ\n");
		nread = read(client_fd, buffer, BUF_SIZE-1);
		buffer[nread] = '\0';
		puts(buffer);

		if (strcmp(buffer, "QUIT") == 0){
			printf("Quitting\n");
			break;
		}

		else if (strcmp(buffer, "LIST") == 0){
			printf("*Listing*\n");
			listingFiles(client_fd);
		}
		
		else
			trasnferTCP(client_fd, buffer);
								
		fflush(stdout);
		fflush(stdin);
	}
	close(client_fd);
}

void listingFiles(int client_fd){

	DIR* directory;
	struct dirent *ficheiroAtual;
	int nread;
	char buffer[BUF_SIZE];

	if ((directory = opendir("./Downloadables")) == NULL){
		perror("Unable to read directory");
		exit(1);
	}

	while((ficheiroAtual = readdir(directory)) != NULL){

		if (strcmp(ficheiroAtual->d_name, ".") != 0 && strcmp(ficheiroAtual->d_name, "..") != 0){
			
			write(client_fd, ficheiroAtual->d_name, strlen(ficheiroAtual->d_name));
			
			memset(buffer, 0, sizeof(buffer));
			nread = read(client_fd, buffer, BUF_SIZE-1);
			buffer[nread] = '\0';

			if (strcmp("check", buffer) != 0)
				exit(1);
		}

		fflush(stdout);
		fflush(stdin);
	}

	write(client_fd, "end", 4);
	closedir(directory);
}

void erro(char* msg)
{
	printf("Erro: %s\n", msg);
	exit(-1);
}
/*
int processDownload(int client_fd, char* buffer){

	int nread;
	char command[100], TCPUDP[100], encryption[100] , file[100];
	
	if (sscanf(buffer, "%s %s %s %s", command, TCPUDP, encryption, file) == EOF){
		write(client_fd, "1", 2);	
		return 1;
	}
	
	if (strcmp(command, "DOWNLOAD") != 0 || (strcmp(TCPUDP, "TCP") != 0 && strcmp(TCPUDP, "UDP") != 0 ) || (strcmp(encryption, "ENC") != 0 && strcmp(encryption, "NOR") != 0)){
		write(client_fd, "1", 2);	
		return 1;
	} 

	write(client_fd, "0", 2);
	
	nread = read(client_fd, buffer, BUF_SIZE-1);
	buffer[nread] = '\0';

	if (strcmp(buffer, "check") != 0)
		exit(1);


	trasnferTCP(client_fd, file, 0);

	return 0;
}*/

void trasnferTCP(int client_fd, char* buf){

	char buffer[BUF_SIZE];
	char dir[2*BUF_SIZE];
	char file[BUF_SIZE];
	int encryption;
	int aux;

	sscanf(buffer, "%s,%d", file, &encryption);
	puts(file);
	write(client_fd, file, strlen(file) + 1);

	sprintf(dir, "./Downloadables/%s", file);
	FILE* f = fopen(dir, "rb");

	while(1){

		aux = fread(buffer, 1, BUF_SIZE, f);

		if (aux == 0)
			break;

 		write(client_fd, buffer, aux);

	}

	fclose(f);
	printf("End of transfer\n");
}