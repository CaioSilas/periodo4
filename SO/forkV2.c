#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <sys/wait.h>

#define N 1000
#define READ 0
#define WRITE 1

int main() {

	pid_t pid;
	int saldo = 0;
	int fdImpressao[2], fdInc[2], fdDec[2], fdLeitura[2];

	if (pipe(fdImpressao) == -1) { // usado para passar o valor para a impressão
		printf("Pipe 1 failed\n");
		exit(EXIT_FAILURE);
	}

	if (pipe(fdInc) == -1) { // usado para passar o valor do incremento para o decremento
		printf("Pipe 2 failed\n");
		exit(EXIT_FAILURE);
	}

	if (pipe(fdDec) == -1) { // usado para passar o valor do decremento para o incremento
		printf("Pipe 3 failed\n");
		exit(EXIT_FAILURE);
	}

	if (pipe(fdLeitura) == -1) { // usado pelo teclado
		printf("Pipe 3 failed\n");
		exit(EXIT_FAILURE);
	}

	write(fdInc[WRITE], &saldo, sizeof(int));
	//write(fdDec[WRITE], &saldo, sizeof(int));

	pid = fork();

	if (pid < 0) {
		printf("Fork 1 failed\n");
		exit(EXIT_FAILURE);
	}

	if (pid == 0) { // Processo para impressão
		printf("Processo saldo: PID: %d\n", getpid());
		close(fdImpressao[WRITE]);
		close(fdInc[READ]); close(fdInc[WRITE]);
		close(fdDec[READ]); close(fdDec[WRITE]);

		for (int i = 0; i < N; i++) {
			read(fdImpressao[READ], &saldo, sizeof(int));
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
			close(fdInc[READ]);
			close(fdDec[WRITE]);

			for (int i = 0; i < N; i++) {
				read(fdDec[READ], &saldo, sizeof(int));
				saldo += 100;
				write(fdInc[WRITE], &saldo, sizeof(int)); // pro decremento
				write(fdImpressao[WRITE], &saldo, sizeof(int)); // pra impressão
				//printf("%d\n", saldo);
			}

			close(fdImpressao[WRITE]);
			close(fdInc[WRITE]);
			close(fdDec[READ]); 
		} else { // Processo para decrementar o saldo
			printf("Processo subtração: PID: %d\n", getpid());
			close(fdImpressao[READ]);
			close(fdInc[WRITE]);
			close(fdDec[READ]);

			for (int i = 0; i < N; i++) {
				read(fdInc[READ], &saldo, sizeof(int));
				saldo -= 100;
				write(fdDec[WRITE], &saldo, sizeof(int));
				write(fdImpressao[WRITE], &saldo, sizeof(int));
				//printf("%d\n", saldo);
			}

			close(fdImpressao[WRITE]);
			close(fdInc[READ]);
			close(fdDec[WRITE]);

			while (wait(NULL) > 0);
		}
	}

	pid = fork();

	if (pid < 0) {
		printf("Fork 1 failed\n");
		exit(EXIT_FAILURE);
	}

	if (pid == 0) { // Processo para impressão
		printf("Processo saldo: PID: %d\n", getpid());
		close(fdImpressao[WRITE]);
		close(fdInc[READ]); close(fdInc[WRITE]);
		close(fdDec[READ]); close(fdDec[WRITE]);
		close(fdLeitura[READ]); close(fdLeitura[WRITE]);

		for (int i = 0; i < N; i++) {
			read(fdImpressao[READ], &saldo, sizeof(int));
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
			close(fdImpressao[READ]); close(fdImpressao[WRITE]);
			close(fdDec[READ]); close(fdDec[WRITE]); 
			close(fdInc[WRITE]);
			close(fdLeitura[READ]);

			for (int i = 0; i < N; i++) {
				read(fdInc[READ], &saldo, sizeof(int));
				saldo += 100;
				write(fdLeitura[WRITE], &saldo, sizeof(int)); // pra leitura
				//printf("%d\n", saldo);
			}

			close(fdInc[READ]);
			close(fdLeitura[WRITE]);
		} else { // Processo para incrementar o saldo
			pid = fork();

			if (pid < 0) {
				printf("Fork 3 failed\n");
				exit(EXIT_FAILURE);
			}

			if (pid == 0) { // Processo para decrementar o saldo
				printf("Processo subtração: PID: %d\n", getpid());
				close(fdImpressao[READ]); close(fdImpressao[WRITE]);
				close(fdDec[WRITE]); 
				close(fdInc[READ]); close(fdInc[WRITE]);
				close(fdLeitura[READ]);

				for (int i = 0; i < N; i++) {
					read(fdDec[READ], &saldo, sizeof(int));
					saldo += 100;
					write(fdLeitura[WRITE], &saldo, sizeof(int)); // pra leitura
					//printf("%d\n", saldo);
				}

				close(fdInc[READ]);
				close(fdLeitura[WRITE]);
			} else { // Processo para ler o teclado
				printf("Processo leitura dos dados: PID: %d\n", getpid());
				close(fdImpressao[READ]);
				close(fdInc[READ]);
				close(fdDec[READ]);
				close(fdLeitura[WRITE]);

				char opcao = "0";

				while (opcao != "s") {
					printf("\n\tDigite \"+\" para incrementar o saldo em 100.");
					printf("\n\tDigite \"-\" para decrementar o saldo em 100.");
					printf("\n\tDigite \"s\" para sair do programa.");
					scanf("%c", &opcao);

					if () {

					}
				}

				close(fdImpressao[READ]);
				close(fdInc[READ]);
				close(fdDec[READ]);
				close(fdLeitura[WRITE]);

				while (wait(NULL) > 0);
			}		
		}
	}

	//exit(EXIT_SUCCESS);
	return 0;
}