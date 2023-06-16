#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>    /* POSIX Threads */ 
#include <stdlib.h>

#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <arpa/inet.h> // htons(), inet_addr()
#include <sys/types.h>  

typedef struct{
    char tabuleiro[3][3];
    int player;
    int sock;
    int verificao;
    int vencedor;
} velhaData;

void imprimeTabuleiro(char tabuleiro[3][3]) {
    printf("\n  0 1 2\n");
    for (int i = 0; i < 3; i++) {
        printf("%d %c|%c|%c\n", i, tabuleiro[i][0], tabuleiro[i][1], tabuleiro[i][2]);
        if (i != 2) {
            printf("  -----\n");
        }
    }
}

int main(){
    int clientSocket;
    struct sockaddr_in serverAddr;
    velhaData data;

    clientSocket = socket(PF_INET, SOCK_STREAM, 0);//inicializa o socket do cliente

    serverAddr.sin_family = AF_INET;//conexao com a internet
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

    //O jogador e o player 1 ou o player 2
    recv(clientSocket, &data, sizeof(data), 0);

    int linha, coluna;
    int jogadaInvalida;
    while(1){
        system("clear");
        if(data.player == 1)
            printf("\nVoce e o X\n");
        else    
            printf("\nVoce e o O\n");
        imprimeTabuleiro(data.tabuleiro);
        recv(clientSocket, &data, sizeof(data), 0); //sua vez de jogar

        system("clear");
        if(data.player == 1)
            printf("\nVoce e o X\n");
        else    
            printf("\nVoce e o O\n");
        imprimeTabuleiro(data.tabuleiro);

        if(data.verificao == 1){
            //informando o resultado final do jogo
            if(data.vencedor == 0)
                printf("\nVelha!");
            else if(data.vencedor == data.player)
                printf("\nVoce Venceu!");
            else
                printf("\nVoce Perdeu :(");

            //verificando se o player quer jogar novamente
            char sincronizar[4];
            recv(clientSocket, sincronizar, sizeof(sincronizar), 0);

            //recebendo resposta do usuario
            printf("\nQuer jogar novamente? (s ou n)");
            char c;
            getchar();
            scanf("%c", &c);
            int resposta = c == 'n' || c == 'N' ? 0 : 1;

            //enviar resposta ao servidor
            send(clientSocket, &resposta, sizeof(resposta), 0);
            
            //recebendo a resposta final do servidor
            recv(clientSocket, &resposta, sizeof(resposta), 0);

            if (!resposta)
                break;
        }

        else{
            do{
                printf("\nSua jogada (linha coluna): ");
                scanf("%d %d", &linha, &coluna);
                jogadaInvalida = linha < 0 || linha > 2 || coluna < 0 || coluna > 2 || data.tabuleiro[linha][coluna] != ' ';
                if(jogadaInvalida)
                    printf("\nJogada Invalida! Jogue novamente");
            }while(jogadaInvalida);

            data.tabuleiro[linha][coluna] = data.player == 1 ? 'X' : 'O';

            system("clear");
            imprimeTabuleiro(data.tabuleiro);

            //envia a jogada para o servidor
            send(clientSocket, &data, sizeof(data), 0);

        }

    }

    close(clientSocket);
    return 0;
    
}