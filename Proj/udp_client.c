#include syssocket.h
#include systypes.h
#include netinetin.h
#include unistd.h
#include stdlib.h
#include stdio.h
#include netdb.h
#include string.h
#include systypes.h
#include syswait.h

#include fcntl.h
#include sysstat.h
#include sysmman.h
#include semaphore.h
#include sysipc.h
#include netinetin.h
#include arpainet.h

#define _OE_SOCKETS

#define BUF_SIZE 512	 Tamanho do buffer
#define PORT 9000	 Porto para recepção das mensagens

void erro(char s)
	{
	perror(s);
	exit(1);
	}

int main(int argc, char argv[]){

	int fd_client;
	int recv_len;
	int send_len;

	char request[BUF_SIZE];
	char resposta[BUF_SIZE];
	char server[100];

	struct sockaddr_in socket_server;
	struct sockaddr_in socket_client;

	struct hostent hostPtr;
	socklen_t slen = sizeof(socket_server);

	if(argc!=4){
		printf(cliente host port stringn);
		exit(-1);
	}
	strcpy(server,argv[1]);
	if ((hostPtr = gethostbyname(server)) == 0)
		 erro(Nao consegui obter endereço);

	bzero((void ) &socket_server, sizeof(socket_server));
	socket_server.sin_family = AF_INET;
	socket_server.sin_addr.s_addr = ((struct in_addr )(hostPtr-h_addr))-s_addr;
	socket_server.sin_port = htons((short) atoi(argv[2]));
	 Cria um socket para recepção de pacotes UDP
	if((fd_client=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		erro(Erro na criação do socket);
	}

	socket_client.sin_family = AF_INET;
	socket_client.sin_port = htons(PORT);
	socket_client.sin_addr.s_addr = htonl(INADDR_ANY);

	 Associa o socket à informação de endereço
	if(bind(fd_client,(struct sockaddr)&socket_client, sizeof(socket_client)) == -1)
	{
		erro(Erro no bind);
	}
	strcpy(request,argv[3]);
	enviar request
	if((send_len=sendto(fd_client,request,BUF_SIZE,0,(struct sockaddr) &socket_server,(socklen_t) slen))==-1){
		erro(Erro no sendto);
	}
	printf(Enviado %sn,request);

	receber resposta
	if((recv_len = recvfrom(fd_client, resposta, BUF_SIZE, 0, (struct sockaddr ) &socket_server, (socklen_t )&slen)) == -1){
		erro(Erro no recvfrom);
	}
	resposta[recv_len]='0'; cortar lixo

	char endereco[INET_ADDRSTRLEN];
	inet_ntop(AF_INET,&(socket_server.sin_addr),endereco,INET_ADDRSTRLEN);

	printf(Recebido %sn , resposta);

	 Fecha socket e termina programa
	close(fd_client);
	return 0;
}

