#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <sys/wait.h>


#define INCREMENTO 500



int main(){
    
    int saldo = 0;
    int fdimpressao[2],fdprocesso[2],fdsoma[2],fdsubtracao[2];
    int pid;

    if (pipe(fdimpressao) < 0) { // usado para passar o valor do incremento para o decremento
		printf("Pipe 1 failed\n");
		exit(EXIT_FAILURE);
	}

    if (pipe(fdsoma) < 0) { // usado para passar o valor do incremento para o decremento
		printf("Pipe 2 failed\n");
		exit(EXIT_FAILURE);
	}

    if (pipe(fdsubtracao) < 0) { // usado para passar o valor do incremento para o decremento
		printf("Pipe 3 failed\n");
		exit(EXIT_FAILURE);
	}

    if (pipe(fdprocesso) < 0) { // usado para passar o valor do incremento para o decremento
		printf("Pipe 4 failed\n");
		exit(EXIT_FAILURE);
	}
    
    saldo = 0;
    write(fdprocesso[1],&saldo,sizeof(int));

    pid = fork();

    if (pid < 0){
        printf("fork 1 failed\n");
        exit(EXIT_FAILURE);
    }

    if (pid == 0){
        //fork do filho
        pid = fork();
        if (pid == 0)
        {
            //impressao
            printf("pid = %d\n",getpid());
            sleep(1);
            while (1)
            {
                read(fdimpressao[0],&saldo,sizeof(int));
                if (saldo == -1){
                    break;
                }
                printf("saldo = %d\n",saldo);
                write(fdprocesso[1],&saldo,sizeof(int));
            }
        }
         else{
            //fork menu
            printf("pid = %d\n",getpid());
            sleep(1);

            char flag = 1;

            while (flag != '0')
            {
                read(fdprocesso[0],&saldo,sizeof(int));
                printf("use: + para somar,- para subtrair e 0 para sair\n");
                scanf("%c",&flag);
                getchar();

                if (flag == '+'){
                    write(fdsoma[1],&saldo,sizeof(int));
                }
                else if (flag == '-'){
                    write(fdsubtracao[1],&saldo,sizeof(int));
                }
                else if(flag == '0'){
                    saldo = -1;

                    write(fdsoma[1],&saldo,sizeof(int));
                    write(fdsubtracao[1],&saldo,sizeof(int));
                    write(fdimpressao[1],&saldo,sizeof(int));
                }

            }
        }
    }
   
    else
    {
        //fork do pai
        pid = fork();

        if(pid < 0){
            printf("Erro fork pai\n");
            exit(EXIT_FAILURE);
        }

        if (pid == 0)//soma
        {
            printf("pid = %d\n",getpid());
            sleep(1);

            while (1)
            {
                read(fdsoma[0],&saldo,sizeof(int));
                if (saldo == -1){
                    break;
                }
                // printf("soma\n");
                saldo += INCREMENTO;
                write(fdimpressao[1],&saldo,sizeof(int));
            }
            
        }
        else//subtracao
        {
            printf("pid = %d\n",getpid());
            sleep(1);
            while (1)
            {
                read(fdsubtracao[0],&saldo,sizeof(int));
                if (saldo == -1){
                    break;
                }
                // printf("subtracao\n");
                saldo -= INCREMENTO;
                
                write(fdimpressao[1],&saldo,sizeof(int));
            }
            
        }




    }
    close(fdimpressao[1]);
    close(fdimpressao[0]);

    close(fdsoma[1]);
    close(fdsoma[0]);

    close(fdsubtracao[1]);
    close(fdsubtracao[0]);

    return 0;
}