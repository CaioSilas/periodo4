// gcc client.c -o client


/****************** CLIENT CODE ****************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <arpa/inet.h>    // htons(), inet_addr()
#include <sys/types.h>    // AF_INET, SOCK_STREAM

int main(){
  int clientSocket;
  char buffer[1024], buffer2[1024];
  struct sockaddr_in serverAddr;
  socklen_t addr_size;

  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(7891);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // colocar o IP da máquina servidora se em máquina diferente
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  addr_size = sizeof serverAddr;
  connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  do {

    printf("Digite alguma coisa (para sair, digite quit): \n");

    scanf("%s]", buffer);
    strcpy(buffer2, buffer);

    send(clientSocket,buffer,sizeof(buffer),0);

    recv(clientSocket, buffer, sizeof(buffer), 0);

    printf("\nData received: %s\n",buffer);   


  } while (strcmp(buffer2, "quit") != 0);
 
  printf("Fechando Conexao e encerrando o programa...\n"); 
  close(clientSocket);

  return 0;
}
