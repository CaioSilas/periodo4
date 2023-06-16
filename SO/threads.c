#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 1000
#define INCREMENTO 100
#define DECREMENTO 100

sem_t impressaoInicio;
sem_t impressaoFim;
sem_t processo;

void *mostraSaldo(void *arg) {
	printf("TID impressão: %ld\n", pthread_self());

	int *saldo = (int*)arg;

	for (int i = 0; i < 2*N; i++) {
	    sem_wait(&impressaoInicio);
		sem_wait(&processo);
		printf("saldo: %d\n", *saldo);
		sem_post(&processo);
		sem_post(&impressaoFim);
	}

	pthread_exit(NULL);
}

void *incrementaSaldo(void *arg) {
	printf("TID incrementa: %ld\n", pthread_self());

	int *saldo = (int*)arg;

	for (int i = 0; i < N; i++) {
		sem_wait(&impressaoFim);
		sem_wait(&processo);
		(*saldo) += INCREMENTO;
		sem_post(&impressaoInicio);
		sem_post(&processo);
	}

	pthread_exit(NULL);
}

void *decrementaSaldo(void *arg) {
	printf("TID decrementa: %ld\n", pthread_self());

	int *saldo = (int*)arg;

	for (int i = 0; i < N; i++) {
		sem_wait(&impressaoFim);
		sem_wait(&processo);
		(*saldo) -= DECREMENTO;
		sem_post(&impressaoInicio);
		sem_post(&processo);
	}

	pthread_exit(NULL);
}


int main() {

	pthread_t tid1, tid2, tid3;
	int saldo = 0;

	sem_init(&impressaoInicio, 0, 0);
	sem_init(&impressaoFim, 0, 1);
	sem_init(&processo, 0, 1);

	if (pthread_create(&tid1, NULL, mostraSaldo, (void*)&saldo)) {
		printf("Erro - create mostraSaldo");
		exit(EXIT_FAILURE);
	}
	if (pthread_create(&tid2, NULL, incrementaSaldo, (void*)&saldo)) {
		printf("Erro - create incrementaSaldo");
		exit(EXIT_FAILURE);
	}
	if (pthread_create(&tid3, NULL, decrementaSaldo, (void*)&saldo)) {
		printf("Erro - create decrementaSaldo");
		exit(EXIT_FAILURE);
	}

	//pthread_exit(NULL);
	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	pthread_join(tid3, NULL);

	sem_destroy(&impressaoInicio);
	sem_destroy(&impressaoFim);
	sem_destroy(&processo);

	printf("Terminou a main.\n");

	return 0;
}