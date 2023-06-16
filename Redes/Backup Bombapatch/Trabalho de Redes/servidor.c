#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <arpa/inet.h>    // htons(), inet_addr()
#include <sys/types.h>    // AF_INET, SOCK_STREAM

typedef struct{
    char tabuleiro[3][3];
    int player;
    int sock;
    int verificacao;
    int vencedor;
} velhaData;

void atualizaDados(velhaData *fonte, velhaData *destino){
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            destino->tabuleiro[i][j] = fonte->tabuleiro[i][j];

    destino->verificacao = fonte->verificacao; 
}

int verificaTabuleiro(char tabuleiro[3][3]) {
    // verifica as linhas
    for (int i = 0; i < 3; i++) {
        if (tabuleiro[i][0] == tabuleiro[i][1] && tabuleiro[i][1] == tabuleiro[i][2] && tabuleiro[i][0] != ' ') {
            return 1;
        }
    }
    // verifica as colunas
    for (int j = 0; j < 3; j++) {
        if (tabuleiro[0][j] == tabuleiro[1][j] && tabuleiro[1][j] == tabuleiro[2][j] && tabuleiro[0][j] != ' ') {
            return 1;
        }
    }
    // verifica as diagonais
    if (tabuleiro[0][0] == tabuleiro[1][1] && tabuleiro[1][1] == tabuleiro[2][2] && tabuleiro[0][0] != ' ') {
        return 1;
    }
    if (tabuleiro[0][2] == tabuleiro[1][1] && tabuleiro[1][1] == tabuleiro[2][0] && tabuleiro[0][2] != ' ') {
        return 1;
    }
    return 0;
}

int main(){
    srand(time(NULL));

    int continuarPartida1, continuarPartida2;
    int welcomeSocket, newSocket;
    
    struct sockaddr_in serverAddr;

    velhaData data[2];

    //inicializando os tabuleiros
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            data[0].tabuleiro[i][j] = ' ';
            data[1].tabuleiro[i][j] = ' ';
        }
    }

    welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);//inicializa o socket do servidor

    serverAddr.sin_family = AF_INET; //conexao com a internet
    serverAddr.sin_port = htons(7891); //porta

    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //endereço de IP
    
    //colocando um \0 no fim do endereco do servidor
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

    //associando o endereco e a porta ao socket
    bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

    // informando que o socket irá ouvir até duas conexões (dois jogadores)
    if(listen(welcomeSocket,2) == 0)
      printf("Listening\n");
    else
      printf("Error\n");

    //recebendo as conexoes
    for(int i = 1; i <= 2; i++){
        newSocket = accept(welcomeSocket, (struct sockaddr *) NULL, NULL); // retorna o id do socket
        printf("\nJogador %d Conectou!", i);
        data[i - 1].player = i;
        data[i - 1].sock = newSocket;
        data[i - 1].verificacao = 0;
    }

    //informando inicio para os clientes
    send(data[0].sock, &data[0], sizeof(data[0]), 0);
    send(data[1].sock, &data[1], sizeof(data[1]), 0);


    int continuarJogo;
    do {
        printf("\n\n\nNovo jogo Comecando!\n");
        int jogada = 1;
        int turno = rand() % 2;

        //inicializando os tabuleiros
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                data[0].tabuleiro[i][j] = ' ';
                data[1].tabuleiro[i][j] = ' ';
            }
        }

        data[0].verificacao = 0;
        data[1].verificacao = 0;

        //o jogo da velha possui no maximo nove rodadas
        while (jogada <= 9){
            //enviando os dados para o jogador da vez
            send(data[turno].sock, &data[turno], sizeof(data[turno]), 0);
            
            //recebendo jogada do jogador
            recv(data[turno].sock, &data[turno], sizeof(data[turno]), 0);

            int verificacao = verificaTabuleiro(data[turno].tabuleiro); 
            data[turno].verificacao = verificacao;

            //reescrevendo tabuleiro do outro jogador
            if(turno)
                atualizaDados(&data[1], &data[0]);
            else
                atualizaDados(&data[0], &data[1]);

            //armazenando o vencedor
            if(verificacao == 1){
                data[0].vencedor = turno + 1;
                data[1].vencedor = turno + 1;
                break;
            }

            turno = turno == 1 ? 0 : 1;
            jogada++;
        }
        //caso de empate
        if(jogada > 9){
            data[0].vencedor = 0;
            data[1].vencedor = 0; 

            data[0].verificacao = 1;
            data[1].verificacao = 1; 
        }

        // enviando os dados de fim da partida
        send(data[0].sock, &data[0], sizeof(data[0]), 0);
        send(data[1].sock, &data[1], sizeof(data[1]), 0);

        char sincronizar[] = "---";

        //X responde primeiro se quer continuar
        send(data[0].sock, sincronizar, sizeof(sincronizar), 0);
        recv(data[0].sock, &continuarPartida1, sizeof(int), 0);

        //O responde segundo se quer continuar
        send(data[1].sock, sincronizar, sizeof(sincronizar), 0);
        recv(data[1].sock, &continuarPartida2, sizeof(int), 0);

        //Informa se o jogo continua ou nao
        continuarJogo = continuarPartida1 && continuarPartida2;
        send(data[0].sock, &continuarJogo, sizeof(int), 0);
        send(data[1].sock, &continuarJogo, sizeof(int), 0);

    } while(continuarJogo);

    printf("\nEncerrando conexoes\n");
    //encerrar conexao
    close(data[0].sock);
    close(data[1].sock);
    close(welcomeSocket);
    
    return 0;
}