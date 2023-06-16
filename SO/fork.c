#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <sys/wait.h>

#define N 10
#define INCREMENTO 10
#define DECREMENTO 5
#define READ 0
#define WRITE 1

int main() {

	pid_t pid;
	int saldo = 0;
	int fdImpressao[2], fdDecremento[2], fdIncremento[2];

	if (pipe(fdImpressao) == -1) { // usado para passar o valor para a impressão
		printf("Pipe 1 failed\n");
		exit(EXIT_FAILURE);
	}

	if (pipe(fdDecremento) == -1) { // usado para passar o valor do incremento para o decremento
		printf("Pipe 2 failed\n");
		exit(EXIT_FAILURE);
	}

	if (pipe(fdIncremento) == -1) { // usado para passar o valor do decremento para o incremento
		printf("Pipe 3 failed\n");
		exit(EXIT_FAILURE);
	}

	write(fdDecremento[WRITE], &saldo, sizeof(int));
	//write(fdIncremento[WRITE], &saldo, sizeof(int));

	pid = fork();

	if (pid < 0) {
		printf("Fork 1 failed\n");
		exit(EXIT_FAILURE);
	}

	if (pid == 0) { // Processo para impressão
		printf("Processo saldo: PID: %d\n", getpid());
		close(fdImpressao[WRITE]);
		close(fdDecremento[READ]); close(fdDecremento[WRITE]);
		close(fdIncremento[READ]); close(fdIncremento[WRITE]);

		/*for (int i = 0; i < 2*N; i++) {
			read(fdImpressao[READ], &saldo, sizeof(int));
			printf("%d\n", saldo);
		}*/

		while (read(fdImpressao[READ], &saldo, sizeof(int))) {
			printf("%d\n", saldo);
		}

		close(fdImpressao[READ]);
	} else {
		pid = fork();

		if (pid < 0) {
			printf("Fork 2 failed\n");
			exit(EXIT_FAILURE);
		}

		if (pid == 0) { // Processo para incrementar o saldo
			printf("Processo soma: PID: %d\n", getpid());
			close(fdImpressao[READ]);
			close(fdDecremento[READ]);
			close(fdIncremento[WRITE]);

			for (int i = 0; i < N; i++) {
				read(fdIncremento[READ], &saldo, sizeof(int));
				saldo += INCREMENTO;
				write(fdDecremento[WRITE], &saldo, sizeof(int)); // pro decremento
				write(fdImpressao[WRITE], &saldo, sizeof(int)); // pra impressão
				//printf("%d\n", saldo);
			}

			close(fdImpressao[WRITE]);
			close(fdDecremento[WRITE]);
			close(fdIncremento[READ]); 
		} else { // Processo para decrementar o saldo
			printf("Processo subtração: PID: %d\n", getpid());
			close(fdImpressao[READ]);
			close(fdDecremento[WRITE]);
			close(fdIncremento[READ]);

			for (int i = 0; i < N; i++) {
				read(fdDecremento[READ], &saldo, sizeof(int));
				saldo -= DECREMENTO;
				write(fdIncremento[WRITE], &saldo, sizeof(int));
				write(fdImpressao[WRITE], &saldo, sizeof(int));
				//printf("%d\n", saldo);
			}

			close(fdImpressao[WRITE]);
			close(fdDecremento[READ]);
			close(fdIncremento[WRITE]);

			while (wait(NULL) > 0);
		}
	}

	//exit(EXIT_SUCCESS);
	return 0;
}