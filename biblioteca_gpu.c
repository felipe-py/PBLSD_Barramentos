#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define caminhoDriver "/dev/driver_tp01_g02"

//Descritor
static int fd;

//Buffer do usuário para enviar ao Driver
static char buffer_user[21];

//Inteiros que representam dados a serem enviados aos registradores DATA_A e DATA_B
static uint32_t dataA;
static uint32_t dataB;

//Inicializa comunicação com o Driver
int open_Driver(){

    //Abre o arquivo com permissão de escrita e leitura. Se ele já existir, sobreescreve
    if((fd = open(caminhoDriver, O_RDWR | O_CREAT | O_TRUNC | S_IRUSR | S_IWUSR)) == -1) {
        perror("Falha ao abrir o arquivo\n");
        return -1;
    }

    return 0;
}

//Encerra comunicação com o Driver
int close_Driver() {

    //Fecha o arquivo
    if(close(fd) == -1) {
        perror("Falha ao encerrar o arquivo\n");
        return -1;
    }

    return 0;
}

//Função para mudar cor base do background
int setCorBackground_WBR(int azul, int verde, int vermelho){
    
    //Verificações de dados recebidos
    if(azul > 7 || verde > 7 || vermelho > 7){
        perror("Valor acima do permitido\n");
        return -1;
    } 
    
    else if(azul < 0 || verde < 0 || vermelho < 0) {
        perror("Valor abaixo do permitido\n");
        return -1;
    }
    
    dataA = 0b0;
    dataB = (azul << 6) | (verde << 3) | vermelho;

    //Tenta enviar dados para o Driver
    if(preenche_buffer()) {
        perror("Erro ao mudar cor base do background\n");
        return -1;
    }

    return 0;
}

//Função para habilitar ou não determinado sprite na tela (639 x 479)
int setSprite_WBR(int ativaSprite, int cord_x, int cord_y, int offset, int registrador){
    
    //Verificações de dados recebidos
    if(ativaSprite > 1 || cord_x > 639 || cord_y > 479 || offset > 31 || registrador > 31){
        perror("Valor acima do permitido\n");
        return -1;
    } 
    
    else if(ativaSprite < 0 || cord_x < 0 || cord_y < 0 || offset < 0 || registrador < 1) {
        perror("Valor abaixo do permitido\n");
        return -1;
    }
    
    dataA = (registrador << 4) | 0b0000;
    dataB = (ativaSprite << 29) | (cord_x << 19) | (cord_y << 9) | offset;

    //Tenta enviar dados para o Driver
    if(preenche_buffer()) {
        perror("Erro ao exibir sprite\n");
        return -1;
    }

    return 0;
}

//Função para editar (mudar cor) determinado bloco do background (79 x 59)
int editBackground_WBM(int bloco_x, int bloco_y, int azul, int verde, int vermelho){
    int bloco = bloco_y * 80 + bloco_x;
    
    //Verificações de dados recebidos
    if(bloco > 4799 || azul > 7 || verde > 7 || vermelho > 7){
        perror("Valor acima do permitido\n");
        return -1;
    } 

    else if(bloco < 0 || azul < 0 || verde < 0 || vermelho < 0){
        perror("Valor abaixo do permitido\n");
        return -1;
    }
    
    dataA = (bloco << 4) | 0b0010;
    dataB = (azul << 6) | (verde << 3) | vermelho;

    //Tenta enviar dados para o Driver
    if(preenche_buffer()) {
        perror("Erro ao editar bloco do background\n");
        return -1;
    }

    return 0;
}

//Função para desabilitar determinado bloco do background (79 x 59)
int desabilitaBlocoBackground_WBM(int bloco_x, int bloco_y){
    int bloco = bloco_y * 80 + bloco_x;
    
    //Verificações de dados recebidos
    if(bloco > 4799){
        perror("Valor acima do permitido\n");
        return -1;
    } 

    else if(bloco < 0){
        perror("Valor abaixo do permitido\n");
        return -1;
    }
    
    dataA = (bloco << 4) | 0b0010;
    dataB = 0b111111110;

    //Tenta enviar dados para o Driver
    if(preenche_buffer()) {
        perror("Erro ao desabilitar bloco do background\n");
        return -1;
    }

    return 0;
}

//Função para editar ou armazenar um pixel de um sprite na memória de sprites (endereco = pixel)
int editSprite_WSM(int endereco, int azul, int verde, int vermelho){
    
    //Verificações de dados recebidos
    if(endereco > 12799 || azul > 7 || verde > 7 || vermelho > 7){
        perror("Valor acima do permitido\n");
        return -1;
    } 

    else if(endereco < 12799 || azul < 0 || verde < 0 || vermelho < 0){
        perror("Valor abaixo do permitido\n");
        return -1;
    }
    
    dataA = (endereco << 4) | 0b0001;
    dataB = (azul << 6) | (verde << 3) | vermelho;

    //Tenta enviar dados para o Driver
    if(preenche_buffer()) {
        perror("Erro ao editar pixel do sprite\n");
        return -1;
    }

    return 0;
}

//Função para exibir um quadrado na tela (639 x 479)
int setQuadrado_DP(int azul, int verde, int vermelho, int tamanho, int ref_x, int ref_y, int ordem_impressao){
    
    //Verificações de dados recebidos
    if(azul > 7 || verde > 7 || vermelho > 7 || tamanho > 15 || ref_x > 511 || ref_y > 480 || ordem_impressao > 15){
        perror("Valor acima do permitido\n");
        return -1;
    } 

    else if(azul < 0 || verde < 0 || vermelho < 0 || tamanho < 0 || ref_x < 0 || ref_y < 0 || ordem_impressao < 0){
        perror("Valor abaixo do permitido\n");
        return -1;
    }
    
    dataA = (ordem_impressao << 4) | 0b0011;
    dataB = (0b0 << 31) | (azul << 28) | (verde << 25) | (vermelho << 22) | (tamanho << 18) | (ref_y << 9) | ref_x;

    //Tenta enviar dados para o Driver
    if(preenche_buffer()) {
        perror("Erro ao exibir um quadrado\n");
        return -1;
    }

    return 0;
}

//Função para exibir um triângulo na tela (639 x 479)
int setTriangulo_DP(int azul, int verde, int vermelho, int tamanho, int ref_x, int ref_y, int ordem_impressao){
    
    //Verificações de dados recebidos
    if(azul > 7 || verde > 7 || vermelho > 7 || tamanho > 15 || ref_x > 511 || ref_y > 480 || ordem_impressao > 15){
        perror("Valor acima do permitido\n");
        return -1;
    } 

    else if(azul < 0 || verde < 0 || vermelho < 0 || tamanho < 0 || ref_x < 0 || ref_y < 0 || ordem_impressao < 0){
        perror("Valor abaixo do permitido\n");
        return -1;
    }
    
    dataA = (ordem_impressao << 4) | 0b0011;
    dataB = (0b1 << 31) | (azul << 28) | (verde << 25) | (vermelho << 22) | (tamanho << 18) | (ref_y << 9) | ref_x;

    //Tenta enviar dados para o Driver
    if(preenche_buffer()) {
        perror("Erro ao exibir um triângulo\n");
        return -1;
    }

    return 0;
}

//Função remover estruturas formadas da tela
int limpar_tela(){
    int i, j;

    //Remove cor do background
    if(setCorBackground_WBR(0,0,0) == -1){
        perror("Erro ao desabilitar cor do background\n");
    }

    //Remove sprites
    for(i=0; i<32; i++){
        if(setSprite_WBR(0,0,0,0,i) == -1){
            perror("Erro ao desabilitar sprites\n");
            break; 
        }
    }

    //Remove Quadrados
    for(i=0; i<16; i++){
        if(setQuadrado_DP(0,0,0,0,0,0,i) == -1){
            perror("Erro ao desabilitar quadrados\n");
            break; 
        }
    }

    //Remove Triângulos
    for(i=0; i<16; i++){
        if(setTriangulo_DP(0,0,0,0,0,0,i) == -1){
            perror("Erro ao desabilitar triangulos\n");
            break; 
        }
    }

    //Remove blocos editados do background
    for (i = 0; i < 80; i++) {
        for (j = 0; j < 60; j++) {
            if(desabilitaBlocoBackground_WBM(i,j) == -1){
                perror("Erro ao desabilitar bloco do background\n");
                break;
            }
        }
    }

    return 0;
}

//Função para transformar inteiro em string ( buffer_user = dataB + dataA )
int preenche_buffer(){
    //Formata o primeiro número para ocupar 10 caracteres iniciais do buffer
    sprintf(buffer_user, "%010" PRIu32, dataB);
    
    //Formata o segundo número para ocupar 10 caracteres finais do buffer
    sprintf(buffer_user + 10, "%010" PRIu32, dataA);

    //Lê arquivo do começo
    lseek(fd, 0, SEEK_SET);

    //Verifica se escreveu no driver
    int erro = write(fd, buffer_user, sizeof(buffer_user));

    return erro;
}