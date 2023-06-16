// gcc ChatClient.c -o ChatClient -lpthread


/****************** CLIENT CODE ****************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>    /* POSIX Threads */ 
#include <stdlib.h>

#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <arpa/inet.h>    // htons(), inet_addr()
#include <sys/types.h>    // AF_INET, SOCK_STREAM



typedef struct str_thdata{
  int sock;
  pthread_t thread;
} thdata;

void threadRecv(void *param){

  thdata *data;
  data = (thdata *) param;
 
  char buffer[1024];

  int s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  if (s != 0) {
    printf("Erro.");
    exit(0);
  }

  while (1){
    recv(data->sock, buffer, sizeof(buffer), 0);

    printf("%s\n",buffer); 
  }

}

void threadSend(void *param){

  thdata *data;
  data = (thdata *)param;

  char msg[1024], nome[100], buffer[1024];

  printf("Digite seu nome: \n");

  fgets(nome, 100, stdin);

  do {

    printf("Digite alguma coisa (para sair, digite quit): \n");

    fgets(msg, 1024, stdin);
    strcpy(buffer, nome);
    strcat(buffer, "-> ");
    strcat(buffer, msg);
    send(data->sock,buffer,sizeof(buffer),0);  


  } while (strcmp(msg, "quit\n") != 0);

  printf("Fechando Conexao e encerrando o programa...\n"); 
  pthread_cancel(data->thread);
  sleep(5);
  shutdown(data->sock, 2);
  pthread_exit(NULL);


}


int main(){
  int clientSocket;
  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  pthread_t threadSendId, threadRecvId;
  thdata dataRecv, dataSend;
  pthread_attr_t attr;

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(7891);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  


  addr_size = sizeof serverAddr;
  connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));


  printf("criando thread recv...\n");
  dataRecv.sock = clientSocket;
  pthread_create (&threadRecvId, &attr, (void *) &threadRecv, (void *) &dataRecv);

  printf("criando thread send...\n");
  dataSend.sock = clientSocket;
  dataSend.thread = threadRecvId;
  pthread_create (&threadSendId, &attr, (void *) &threadSend, (void *) &dataSend);

 
  pthread_join(threadSendId, NULL);
  pthread_join(threadRecvId, NULL);
  

  pthread_attr_destroy(&attr);

  printf("Todas as threads terminaram. Fechando cliente.\n");


  return 0;
}
