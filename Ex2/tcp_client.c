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

#define BUF_SIZE  1024

void erro(char *msg);
void dados();
void process_server(int server_fd);

int main(int argc, char *argv[]) {
  char endServer[100];
  int fd;
  struct sockaddr_in addr;
  struct hostent *hostPtr;

  if (argc != 3) {
    	printf("cliente <host> <port>\n");
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

/*
void process_server(int server_fd)
{
  int nread = 0;
  //char buffer[BUF_SIZE];
  char buffer2[BUF_SIZE];

  nread = read(server_fd, buffer2, BUF_SIZE-1);
  buffer2[nread] = '\0';
  printf("%s\n", buffer2);
 
  if (strcmp(buffer2,"DADOS") == 0) dados(server_fd);
  //else if (strcmp(buffer,"SOMA") == 0) soma();
  //else if (strcmp(buffer,"MEDIA") == 0) media();

  fflush(stdout);

  close(server_fd);
}
*/

void process_server(int server_fd){
    
    char buffer[BUF_SIZE];
    char temp[BUF_SIZE];;
    char result[BUF_SIZE];
    int nread;
    int i;

    while(1){
        
        memset(buffer, 0, sizeof(buffer));
        scanf("%s", buffer);
        write(server_fd, buffer, strlen(buffer));
        printf("CLIENT 87\n");
        if(strcmp(buffer, "DADOS") == 0){

            printf("insira os dados:\n");

            for(i = 0; i < 10; i++){

                printf("Inserir numero:\n");
                
                scanf("%s", temp);
                printf("CLIENT 97\n");
                write(server_fd, temp, strlen(temp) + 1);
                printf("CLIENT 99\n");
                nread = read(server_fd, result, 18);
                printf("CLIENT 101\n");
                result[nread] = '\0';
                puts(result);
                printf("CLIENT 104\n");
                if(strcmp(result, "Operacao invalida") == 0){
                    puts(result);
                    printf("CLIENT 106\n");
                    break;
                }

                fflush(stdin);
            }
        }

        else{
            printf("CLIENT 115\n");
            nread = read(server_fd, buffer, BUF_SIZE);
            printf("CLIENT 117\n");
            buffer[nread] = '\0';
            printf("%s\n", buffer);
            printf("CLIENT 120\n");
            fflush(stdin);

        }
    }

    fflush(stdin);
    fflush(stdout);
    close(server_fd);
    exit(0);
}

void erro(char *msg) {
	printf("Erro: %s\n", msg);
	exit(-1);
}