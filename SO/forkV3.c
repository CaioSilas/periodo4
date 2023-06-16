#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <sys/wait.h>

#define N 1000
#define INCREMENTO 100
#define DECREMENTO 100
#define READ 0
#define WRITE 1

int main() {

	pid_t pid;
	int saldo = 0;
	int fdImpressao[2], fdProcesso[2];

	if (pipe(fdImpressao) == -1) { // usado para passar o valor para a impressão
		printf("Pipe 1 failed\n");
		exit(EXIT_FAILURE);
	}

	if (pipe(fdProcesso) == -1) { // usado para passar o valor do incremento para o decremento
		printf("Pipe 2 failed\n");
		exit(EXIT_FAILURE);
	}

	write(fdProcesso[WRITE], &saldo, sizeof(int));

	pid = fork();

	if (pid < 0) {
		printf("Fork 1 failed\n");
		exit(EXIT_FAILURE);
	}

	if (pid == 0) { // Filho: Processo para impressão
		printf("Processo saldo: PID: %d\n", getpid());
		close(fdImpressao[WRITE]);
		close(fdProcesso[READ]);

		while (read(fdImpressao[READ], &saldo, sizeof(int))) {
			printf("%d\n", saldo);
			write(fdProcesso[WRITE], &saldo, sizeof(int)); // manda para os processos
		}

		close(fdImpressao[READ]);
		close(fdProcesso[WRITE]);
	} else { // Pai
		pid = fork();

		if (pid < 0) {
			printf("Fork 2 failed\n");
			exit(EXIT_FAILURE);
		}

		if (pid == 0) { // Filho 2: Processo para incrementar o saldo
			printf("Processo soma: PID: %d\n", getpid());
			close(fdImpressao[READ]);
			close(fdProcesso[WRITE]);

			for (int i = 0; i < N; i++) {
				read(fdProcesso[READ], &saldo, sizeof(int));
				saldo += INCREMENTO;
				write(fdImpressao[WRITE], &saldo, sizeof(int)); // manda pra impressão
			}

			close(fdImpressao[WRITE]);
			close(fdProcesso[READ]);
		} else { // Processo para decrementar o saldo
			printf("Processo subtração: PID: %d\n", getpid());
			close(fdImpressao[READ]);
			close(fdProcesso[WRITE]);

			for (int i = 0; i < N; i++) {
				read(fdProcesso[READ], &saldo, sizeof(int));
				saldo -= DECREMENTO;
				write(fdImpressao[WRITE], &saldo, sizeof(int)); // manda pra impressão
			}

			close(fdImpressao[WRITE]);
			close(fdProcesso[READ]);

			while (wait(NULL) > 0);
		}
	}

	//exit(EXIT_SUCCESS);
	return 0;
}