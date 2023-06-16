#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define INCREMENTO 1000
#define DECREMENTO 100

void *mostrasaldo(void *arg){
    printf("TID impressão: %ld\n", pthread_self());

    int *valor = (int*)arg;
    for (int i = 0; i < 2 * 100;i++){
        printf("valor :%d\n",*valor);
    }
    pthread_exit(NULL);
}

void *soma(void *arg){

    printf("TID incrementa: %ld\n", pthread_self());

    int *valor = (int*)arg;
    for (int i = 0; i < 2 * 100;i++){
        *valor += INCREMENTO;
    }
    pthread_exit(NULL);

}

void *subtrai(void *arg){

    printf("TID decremento: %ld\n", pthread_self());
    int *valor = (int*)arg;
    for (int i = 0; i < 2 * 100;i++){
        *valor -= DECREMENTO;
    }
    pthread_exit(NULL);

}


int main (){
    pthread_t tid1, tid2, tid3;
	int saldo = 0;
    
    if (pthread_create(&tid1, NULL, mostrasaldo, (void*)&saldo)) {
		printf("Erro - create mostraSaldo");
		exit(EXIT_FAILURE);
	}
	if (pthread_create(&tid2, NULL, soma, (void*)&saldo)) {
		printf("Erro - create incrementaSaldo");
		exit(EXIT_FAILURE);
	}
	if (pthread_create(&tid3, NULL, subtrai, (void*)&saldo)) {
		printf("Erro - create decrementaSaldo");
		exit(EXIT_FAILURE);
	}


    pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	pthread_join(tid3, NULL);

    printf("fim\n");

    return 0;
}