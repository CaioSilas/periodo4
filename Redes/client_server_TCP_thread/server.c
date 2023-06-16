// gcc server.c -o server -lpthread



/****************** SERVER CODE ****************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <pthread.h>    /* POSIX Threads */ 
#include <stdlib.h>

#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <arpa/inet.h>    // htons(), inet_addr()
#include <sys/types.h>    // AF_INET, SOCK_STREAM

#define NUM_THREADS 4

pthread_cond_t count_threshold;

typedef struct str_thdata
{
  int thread_no;
  int sock;
  char message[100];

} thdata;


void conexao( void *param){

  thdata *data;            
  data = (thdata *) param;  /* type cast to a pointer to thdata */

  char buffer[1024], buffer2[1024];
  do {

    printf("Esperando Mensagem do cliente...\n");
    recv(data->sock, buffer, sizeof(buffer), 0);

    printf("Mensagem recebida do cliente = %s\n", buffer);
    strcpy(buffer2, buffer);
    strcat(buffer, " -- mensagem do servidor. ");
    strcat(buffer, data->message);

    printf("enviando mensagem de volta....\n");
    send(data->sock,buffer,sizeof(buffer),0);

  }while (strcmp(buffer2, "quit") != 0);

  printf("fechando conexao...\n");
  close(data->sock);


}


int main(){
  int welcomeSocket, newSocket;

  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  pthread_t threads[NUM_THREADS];
  thdata data[NUM_THREADS];

  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(7891); // lembrar de alterar se necessário

  // lembrar de alterar se o servidor e cliente estiverem em máquinas diferentes. Nesse caso, colocar o IP da máquina que será servidora.
  // o IP 127.0.0.1 só funciona se cliente e servidor estiverem na mesma máquina
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  time_t ticks; 

  // servidor irá esperar até NUM_THREADS conexões
  if(listen(welcomeSocket,NUM_THREADS)==0)
    printf("Listening\n");
  else
    printf("Error\n");

  // esperando até NUM_THREADS conexões
  int i = 0;
  while (i < NUM_THREADS) {

    printf("esperando conexao do cliente.... \n");
    newSocket = accept(welcomeSocket, (struct sockaddr *) NULL, NULL);

    data[i].thread_no = i;
    data[i].sock = newSocket;
    strcpy(data[i].message, "thread id = ");
    char str[10];
    sprintf(str, "%d", i);
    strcat(data[i].message, str);

    pthread_create (&threads[i], NULL, (void *) &conexao, (void *) &data[i]);
    printf ("cliente conectou.\n");
    i++;
  }


  printf("Abriu todas as threads. Esperando a thread terminar para fechar o servidor.\n");
  // servidor só irá terminar a sua execução quando todos os clientes digitarem 'quit'
  for (i = 0; i < NUM_THREADS; i++){
    pthread_join(threads[i], NULL);
  }



  return 0;
}
