#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define N 10
int main(){

    //variável saldo
    int saldo;

    //1 pipe pra cada requisito
    int somaPipe[2];
    int subtraiPipe[2];
    int imprimePipe[2];
    int menuPipe[2];

    //Debug nos pipes
    int rPipe = pipe(menuPipe);
    if(rPipe < 0){
        printf("Erro\n");
        exit(EXIT_FAILURE);
    }

    rPipe = pipe(somaPipe);
    if(rPipe < 0){
        printf("Erro\n");
        exit(EXIT_FAILURE);
    }

    rPipe = pipe(subtraiPipe);
    if(rPipe < 0){
        printf("Erro\n");
        exit(EXIT_FAILURE);
    }

    rPipe = pipe(imprimePipe);
    
    if(rPipe < 0){
        printf("Erro\n");
        exit(EXIT_FAILURE);
    }

    saldo = 0;
    write(menuPipe[1], &saldo, sizeof(int));
    //Crio e testo o fork
    int rFork = fork();

    if(rFork < 0){
        printf("Erro\n");
        exit(EXIT_FAILURE);
    }
    if(rFork == 0){
        
        rFork = fork();
        if(rFork < 0){
            printf("Erro\n");
            exit(EXIT_FAILURE);
        }
        
        if(rFork == 0){
            //Fork de impressão 
            printf("PID = %d\n", getpid());
            
            while(1){
                read(imprimePipe[0], &saldo, sizeof(int));
                if(saldo == -1){
                    break;
                }
                printf("Saldo = %d\n", saldo);
                write(menuPipe[1], &saldo, sizeof(int));

            }
        }
        else{
            //Fork do menu 
            printf("PID = %d\n", getpid());
            char op = -1;
            while(op != '0'){
                read(menuPipe[0], &saldo, sizeof(int));

                printf("Escolha + - ou 0 para sair\n");
                scanf("%c", &op);
                getchar();

                if(op == '+'){
                    write(somaPipe[1], &saldo, sizeof(int));
                }
                else if(op == '-'){
                    write(subtraiPipe[1], &saldo, sizeof(int));

                }
                else if(op == '0'){
                    saldo = -1;
                    write(somaPipe[1], &saldo, sizeof(int));
          
                    write(subtraiPipe[1], &saldo, sizeof(int));
          
                    write(imprimePipe[1], &saldo, sizeof(int));

                }
            }
        }
        
    }
    else{
        //Crio e testo o fork do pai
        rFork = fork();

        if(rFork < 0){
            printf("Erro\n");
            exit(EXIT_FAILURE);
        }
        //Fork de soma
        if(rFork == 0){
            printf("PID = %d\n", getpid());
            while(1){
                read(somaPipe[0], &saldo, sizeof(int));
                if(saldo == -1){
                    break;
                }
                saldo += 500;
                write(imprimePipe[1], &saldo, sizeof(int));
            }
        }
        //Fork de subtração
        else{
            printf("PID = %d\n", getpid());
            while(1){
                read(subtraiPipe[0], &saldo, sizeof(int));
                if(saldo == -1){
                    break;
                }
                saldo -= 500;
                write(imprimePipe[1], &saldo, sizeof(int));
            }
        }
    }

    close(somaPipe[0]);
    close(somaPipe[1]);

    close(imprimePipe[0]);
    close(imprimePipe[1]);

    close(subtraiPipe[0]);
    close(subtraiPipe[1]);
    
    return 0;
}