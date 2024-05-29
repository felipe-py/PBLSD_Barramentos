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
void habilitaLeitura(volatile int *WRREG_PTR){
    *WRREG_PTR = 1;
    *WRREG_PTR = 0;
}

//Função para mudar cor base do background
void setCorBackground_WBR(volatile int *WRREG_PTR, volatile int *DATA_A_PTR, volatile int *DATA_B_PTR, int b, int g, int r){
    *DATA_A_PTR = 0b0;
    *DATA_B_PTR = (b << 6) | (g << 3) | r;

    habilitaLeitura(WRREG_PTR);
}

//Função para habilitar ou não determinado sprite na tela (639 x 479)
void setSprite_WBR(volatile int *WRREG_PTR, volatile int *DATA_A_PTR, volatile int *DATA_B_PTR, int ativaSprite, int cord_x, int cord_y, int offset, int registrador){
    *DATA_A_PTR = (registrador << 4) | 0b0000;
    *DATA_B_PTR = (ativaSprite << 29) | (cord_x << 19) | (cord_y << 9) | offset;

    habilitaLeitura(WRREG_PTR);
}

//Função para editar (mudar cor) determinado bloco do background (79 x 59)
void editBackground_WBM(volatile int *WRREG_PTR, volatile int *DATA_A_PTR, volatile int *DATA_B_PTR, int bloco_x, int bloco_y, int b, int g, int r){
    int bloco = bloco_y * 80 + bloco_x;

    *DATA_A_PTR = (bloco << 4) | 0b0010;
    *DATA_B_PTR = (b << 6) | (g << 3) | r;

    habilitaLeitura(WRREG_PTR);
}

//Função para desabilitar determinado bloco do background (79 x 59)
void desabilitaBlocoBackground_WBM(volatile int *WRREG_PTR, volatile int *DATA_A_PTR, volatile int *DATA_B_PTR, int bloco_x, int bloco_y){
    int bloco = bloco_y * 80 + bloco_x;

    *DATA_A_PTR = (bloco << 4) | 0b0010;
    *DATA_B_PTR = 0b111111110;

    habilitaLeitura(WRREG_PTR);
}

//Função para editar ou armazenar um pixel de um sprite na memória de sprites (endereco=pixel)
void editSprite_WSM(volatile int *WRREG_PTR, volatile int *DATA_A_PTR, volatile int *DATA_B_PTR, int endereco, int b, int g, int r){
    *DATA_A_PTR = (endereco << 4) | 0b0001;
    *DATA_B_PTR = (b << 6) | (g << 3) | r;

    habilitaLeitura(WRREG_PTR);
}

//Função para exibir um triângulo na tela (639 x 479)
void setTriangulo_DP(volatile int *WRREG_PTR, volatile int *DATA_A_PTR, volatile int *DATA_B_PTR, int b, int g, int r, int tamanho, int ref_x, int ref_y, int ordem_impressao){
    *DATA_A_PTR = (ordem_impressao << 4) | 0b0011;
    *DATA_B_PTR = (0b1 << 31) | (b << 28) | (g << 25) | (r << 22) | (tamanho << 18) | (ref_y << 9) | ref_x;

    habilitaLeitura(WRREG_PTR);
}

//Função para exibir um quadrado na tela (639 x 479)
void setQuadrado_DP(volatile int *WRREG_PTR, volatile int *DATA_A_PTR, volatile int *DATA_B_PTR, int b, int g, int r, int tamanho, int ref_x, int ref_y, int ordem_impressao){
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

    ///Azul claro (ceu)
    setCorBackground_WBR(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 6, 4, 3); 

//-----------------------------------------------------------------------------------------------------------------------//

    //Gramas verde
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 2, 59, 0, 7, 0);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 1, 59, 0, 7, 0);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 3, 58, 0, 7, 0);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 4, 59, 0, 7, 0);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 4, 58, 0, 7, 0);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 2, 58, 0, 7, 0);

//-----------------------------------------------------------------------------------------------------------------------//

    //Nuvem 1
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 10, 4, 7, 7, 7);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 11, 4, 7, 7, 7);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 12, 4, 7, 7, 7);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 13, 4, 7, 7, 7);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 10, 5, 7, 7, 7);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 11, 5, 7, 7, 7);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 10, 6, 7, 7, 7);

    //Nuvem 2
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 25, 20, 7, 7, 7);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 26, 20, 7, 7, 7);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 27, 20, 7, 7, 7);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 25, 21, 7, 7, 7);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 26, 21, 7, 7, 7);
    
    //Nuvem 3
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 37, 20, 7, 7, 7);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 38, 20, 7, 7, 7);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 39, 20, 7, 7, 7);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 39, 21, 7, 7, 7);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 40, 21, 7, 7, 7);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 40, 22, 7, 7, 7);

    //Nuvem 4
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 58, 15, 7, 7, 7);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 59, 15, 7, 7, 7);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 60, 15, 7, 7, 7);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 59, 16, 7, 7, 7);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 60, 16, 7, 7, 7);
    editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 60, 17, 7, 7, 7);

//-----------------------------------------------------------------------------------------------------------------------//

    //Sprite barra laranja
    setSprite_WBR(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 1, 320, 445, 11, 1);

    //Sprite diamante 1
    setSprite_WBR(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 1, 352, 355, 22, 2);

    //Sprite diamante 2
    setSprite_WBR(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 1, 272, 355, 22, 3);

    //Sprite arvore 1
    setSprite_WBR(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 1, 100, 461, 4, 4);

    //Sprite arvore 2
    setSprite_WBR(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 1, 511, 461, 4, 5);

    //Sprite tronco 2 marrom
    setSprite_WBR(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 1, 471, 463, 16, 6);

    //Sprite tronco 1 marrom
    setSprite_WBR(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 1, 140, 463, 16, 7);

//-----------------------------------------------------------------------------------------------------------------------//

    //Quadrado sol (amarelo)
    setQuadrado_DP(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 0, 6, 6, 6, 511, 60, 0);

    //Triangulo sol (amarelo)
    setTriangulo_DP(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 0, 6, 6, 7, 511, 53, 1);

//-----------------------------------------------------------------------------------------------------------------------//
    
    //Quadrado janela 1 (marrom)
    setQuadrado_DP(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 1, 2, 4, 2, 360, 365, 2);

    //Quadrado janela 2 (marrom)
    setQuadrado_DP(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 1, 2, 4, 2, 280, 365, 3);

//-----------------------------------------------------------------------------------------------------------------------//

    //Quadrado porta base (marrom)
    setQuadrado_DP(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 1, 2, 4, 2, 320, 320, 4);

    //Quadrado porta meio (marrom)
    setQuadrado_DP(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 1, 2, 4, 2, 320, 435, 5);

//-----------------------------------------------------------------------------------------------------------------------//

    //Triangulo teto Casa (marrom)
    setTriangulo_DP(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 1, 2, 4, 3, 245, 299, 6);
    //Triangulo teto Casa (marrom)
    setTriangulo_DP(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 1, 2, 4, 3, 275, 299, 7);
    //Triangulo teto Casa (marrom)
    setTriangulo_DP(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 1, 2, 4, 3, 305, 299, 8);
    //Triangulo teto Casa (marrom)
    setTriangulo_DP(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 1, 2, 4, 3, 335, 299, 9);
    //Triangulo teto Casa (marrom)
    setTriangulo_DP(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 1, 2, 4, 3, 365, 299, 10);
    //Triangulo teto Casa (marrom)
    setTriangulo_DP(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 1, 2, 4, 3, 395, 299, 11);

//-----------------------------------------------------------------------------------------------------------------------//

    //Quadrado parede Casa (branco)
    setQuadrado_DP(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, 7, 7, 7, 15, 320, 400, 12);

//-----------------------------------------------------------------------------------------------------------------------//

    if (munmap(LW_virtual, LW_BRIDGE_SPAN) != 0) {
        printf("ERROR: munmap() failed...\n");
        return (-1);
    }

    close(fd);

    return 0;
}
