#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>

//CONSTANTES
#define LW_BRIDGE_BASE 0xFF200000

#define DATA_A 0x80
#define DATA_B 0x70
#define WRREG 0xc0
#define LW_BRIDGE_SPAN 0x100

//Função para habilitar escrita nas FIFOs
void habilitarLeitura(volatile int *WRREG_PTR){
    *WRREG_PTR = 1;
    *WRREG_PTR = 0;
}

//Função para mudar cor base do background
void setCorBackground_WBR(volatile int *WRREG_PTR, volatile int *DATA_A_PTR, volatile int *DATA_B_PTR, int b, int g, int r){
    *DATA_A_PTR = 0b0;
    *DATA_B_PTR = (b << 6) | (g << 3) | r;

    habilitaLeitura(WRREG_PTR);
}

//Função para habilitar ou não determinado sprite na tela (640 x 480)
void setSprite_WBR(volatile int *WRREG_PTR, volatile int *DATA_A_PTR, volatile int *DATA_B_PTR, int ativaSprite, int cord_x, int cord_y, int offset, int registrador){
    *DATA_A_PTR = (registrador << 4) | 0b0000;
    *DATA_B_PTR = (ativaSprite << 29) | (cord_x << 19) | (cord_y << 9) | offset;

    habilitaLeitura(WRREG_PTR);
}

//Função para editar (mudar cor) determinado bloco do background (80 x 60)
void editBackground_WBM(volatile int *WRREG_PTR, volatile int *DATA_A_PTR, volatile int *DATA_B_PTR, , int bloco_x, int bloco_y, int b, int g, int r){
    int bloco = bloco_y * 80 + bloco_x;

    *DATA_A_PTR = (bloco << 4) | 0b0010;
    *DATA_B_PTR = (b << 6) | (g << 3) | r;

    habilitaLeitura(WRREG_PTR);
}

//Função para desabilitar determinado bloco do background (80 x 60)
void desabilitaBlocoBackground_WBM(volatile int *WRREG_PTR, volatile int *DATA_A_PTR, volatile int *DATA_B_PTR, , int bloco_x, int bloco_y){
    int bloco = bloco_y * 80 + bloco_x;

    *DATA_A_PTR = (bloco << 4) | 0b0010;
    *DATA_B_PTR = 0b111111110;

    habilitaLeitura(WRREG_PTR);
}

//Função para editar ou armazenar um pixel de um sprite na memória de sprites (endereco=pixel)
void editSprite_WSM(volatile int *WRREG_PTR, volatile int *DATA_A_PTR, volatile int *DATA_B_PTR, , int endereco, int b, int g, int r){
    *DATA_A_PTR = (endereco << 4) | 0b0001;
    *DATA_B_PTR = (b << 6) | (g << 3) | r;

    habilitaLeitura(WRREG_PTR);
}

//Função para exibir um triângulo na tela (640 x 480)
void setTriangulo_DP(volatile int *WRREG_PTR, volatile int *DATA_A_PTR, volatile int *DATA_B_PTR, int b, int g, int r, int tamanho, int ref_y, int ref_x, int ordem_impressao){
    *DATA_A_PTR = (ordem_impressao << 4) | 0b0011;
    *DATA_B_PTR = (0b1 << 31) | (b << 28) | (g << 25) | (r << 22) | (tamanho << 18) | (ref_y << 9) | ref_x;

    habilitaLeitura(WRREG_PTR);
}

//Função para exibir um quadrado na tela (640 x 480)
void setQuadrado_DP(volatile int *WRREG_PTR, volatile int *DATA_A_PTR, volatile int *DATA_B_PTR, int b, int g, int r, int tamanho, int ref_y, int ref_x, int ordem_impressao){
    *DATA_A_PTR = (ordem_impressao << 4) | 0b0011;
    *DATA_B_PTR = (0b0 << 31) | (b << 28) | (g << 25) | (r << 22) | (tamanho << 18) | (ref_y << 9) | ref_x;

    habilitaLeitura(WRREG_PTR);
}

int main(void){
    int fd = -1;             
    void *LW_virtual;          

    volatile int *DATA_A_PTR;
    volatile int *DATA_B_PTR;   
    volatile int *WRREG_PTR;

    if ((fd = open("/dev/mem", (O_RDWR | O_SYNC))) == -1) {
        printf("ERROR: could not open \"/dev/mem\"...\n");
        return (-1);
    }

    LW_virtual = mmap(NULL, LW_BRIDGE_SPAN, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, LW_BRIDGE_BASE);
    if (LW_virtual == MAP_FAILED) {
        printf("ERROR: mmap() failed...\n");
        close(fd);
        return (-1);
    }

    DATA_A_PTR = (int *) (LW_virtual + DATA_A);
    DATA_B_PTR = (int *) (LW_virtual + DATA_B);   
    WRREG_PTR = (int *) (LW_virtual + WRREG);

//-----------------------------------------------------------------------------------------------------------------------//

    //Verde claro (grama)
    setCorBackground_WBR(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 1, 6, 2); 

//-----------------------------------------------------------------------------------------------------------------------//
/*
    //ps: alterar y caso fique ruim

    //Loop muda blocos da tela
    for (int bloco_y = 0; bloco_y < 59; bloco_y++) {
        for (int bloco_x = 0; bloco_x < 79; bloco_x++) {
            //Azul claro (ceu)
            desabilitaBlocoBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, bloco_x, bloco_y);
        }
    }

//-----------------------------------------------------------------------------------------------------------------------//
/*

    //Sprite arvore 1
    setSprite_WBR(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 1, 0, 0, 4, 1);

    //Sprite arvore 2
    setSprite_WBR(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 1, 640, 480, 4, 2);

//-----------------------------------------------------------------------------------------------------------------------//
/*

    //Quadrado janela 1 (marrom)
    setQuadrado_DP(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 1, 2, 4, 1, 480, 640, 0);

    //Quadrado janela 2 (marrom)
    setQuadrado_DP(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 1, 2, 4, 1, 0, 0, 1);

//-----------------------------------------------------------------------------------------------------------------------//
*/

    if (munmap(LW_virtual, LW_BRIDGE_SPAN) != 0) {
        printf("ERROR: munmap() failed...\n");
        return (-1);
    }

    close(fd);

    return 0;
}
