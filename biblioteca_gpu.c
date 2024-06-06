#include "biblioteca_gpu.h"
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

static uint32_t data_a, data_b;     /* Inteiros que representam dados a serem enviados aos registradores DATA_A e DATA_B */

static int fd;                      /* Descritor */

static char buffer_user[21];        /* Buffer do usuário para enviar ao Driver */

/**Função para inicializar comunicação com o driver
 * retorno ->       Retorna 0 caso comunicação seja iniciada ou -1 caso dê erro
 */
int 
open_driver() {
    /* Abre o arquivo com permissão de escrita e leitura.*/
    fd = open(CAMINHO_DRIVER, (O_RDWR | O_SYNC));

    if (fd == -1) {
        perror("Falha ao abrir o arquivo\n");

        return -1;
    }

    return 0;
}

/**Função para encerrer comunicação com o driver
 * retorno ->       Retorna 0 caso comunicação seja encerrada ou -1 caso dê erro
 */
int 
close_driver() {
    /* Fecha o arquivo */
    if (close(fd) == -1) {
        perror("Falha ao encerrar o arquivo\n");

        return -1;
    }

    return 0;
}

/**Função para mudar a cor do background
 * parâmetros ->    azul: tom de azul
 *                  verde: tom de verde
 *                  vermelho: tom de vermelho
 * retorno ->       0 caso seja bem sucedido ou -1 caso ocorra algum erro
 */
int 
set_cor_background_wbr(int azul, int verde, int vermelho) {
    /* Verificações de dados recebidos */
    if (azul > 7 || verde > 7 || vermelho > 7) {
        perror("Valor acima do permitido\n");

        return -1;
    } else if (azul < 0 || verde < 0 || vermelho < 0) {
        perror("Valor abaixo do permitido\n");

        return -1;
    }
    
    /* Atribui valores decimais para serem convertidos em string para o buffer */
    data_a = 0b0;
    data_b = (azul << 6) | (verde << 3) | vermelho;

    /* Tenta enviar dados para o Driver */
    if (preenche_buffer()) {
        perror("Erro ao mudar cor base do background\n");

        return -1;
    }

    return 0;
}

/**Função para exibir sprite
 * parâmetros ->    ativa_sprite: visibilidade do sprite, 1 para ativar, 0 para desativar
 *                  cord_x: coordenada X da tela (0 ao 639)
 *                  cord_y: coordenada Y da tela (0 ao 479)
 *                  offset: escolha de um sprite (0 ao 31)
 *                  registrador: registrador onde guardar dados de um sprite (1 ao 31)
 * retorno ->       0 caso seja bem sucedido ou -1 caso ocorra algum erro
 */
int 
set_sprite_wbr(int ativa_sprite, int cord_x, int cord_y, int offset, int registrador) {
    /* Verificações de dados recebidos */
    if (ativa_sprite > 1 || cord_x > 639 || cord_y > 479 || offset > 31 || registrador > 31) {
        perror("Valor acima do permitido\n");

        return -1;
    } else if (ativa_sprite < 0 || cord_x < 0 || cord_y < 0 || offset < 0 || registrador < 1) {
        perror("Valor abaixo do permitido\n");

        return -1;
    }

    /* Atribum vaeslor decismal para emser convertsido em string para o buffer */  
    data_a = (registrador << 4) | 0b0000;
    data_b = (ativa_sprite << 29) | (cord_x << 19) | (cord_y << 9) | offset;

    /* Tenta enviar dados para o Driver */
    if (preenche_buffer()) {
        perror("Erro ao exibir sprite\n");

        return -1;
    }

    return 0;
}

/**Função para editar um bloco do background
 * parâmetros ->    bloco_x: valor da coluna do bloco (0 ao 79)
 *                  bloco_y: valor da linha do bloco (0 ao 59)
 *                  azul: tom de azul
 *                  verde: tom de verde
 *                  vermelho: tom de vermelho
 *                  bloco: variável usada para calcular valor referente ao endereço de memória de um bloco
 * retorno ->       0 caso seja bem sucedido ou -1 caso ocorra algum erro
 */
int 
edit_background_wbm(int bloco_x, int bloco_y, int azul, int verde, int vermelho) {
    int bloco = bloco_y * 80 + bloco_x;
    
    /* Verificações de dados recebidos */
    if (bloco > 4799 || azul > 7 || verde > 7 || vermelho > 7) {
        perror("Valor acima do permitido\n");

        return -1;
    } else if (bloco < 0 || azul < 0 || verde < 0 || vermelho < 0) {
        perror("Valor abaixo do permitido\n");

        return -1;
    }
    
    /* Atribui valores decimais para serem convertidos em string para o buffer */
    data_a = (bloco << 4) | 0b0010;
    data_b = (azul << 6) | (verde << 3) | vermelho;

    /* Tenta enviar dados para o Driver */
    if (preenche_buffer()) {
        perror("Erro ao editar bloco do background\n");

        return -1;
    }

    return 0;
}

/**Função para desabilitar um bloco do background
 * parâmetros ->    bloco_x: valor da coluna do bloco (0 ao 79)
 *                  bloco_y: valor da linha do bloco (0 ao 59)
 *                  bloco: variável usada para calcular valor referente ao endereço de memória de um bloco
 * retorno ->       0 caso seja bem sucedido ou -1 caso ocorra algum erro
 */
int 
desabilita_bloco_background_wbm(int bloco_x, int bloco_y) {
    int bloco = bloco_y * 80 + bloco_x;
    
    /* Verificações de dados recebidos */
    if (bloco > 4799) {
        perror("Valor acima do permitido\n");

        return -1;
    } else if (bloco < 0) {
        perror("Valor abaixo do permitido\n");

        return -1;
    }

    /* Atribum vaeslor decismal para emser convertsido em string para o buffer */
    data_a = (bloco << 4) | 0b0010;
    data_b = 0b111111110;

    /* Tenta enviar dados para o Driver */
    if (preenche_buffer()) {
        perror("Erro ao desabilitar bloco do background\n");

        return -1;
    }

    return 0;
}

/**Função para editar um pixel de um sprite armazenado ou criar um pixel de um novo sprite
 * parâmetros ->    endereco: valor referente ao endereço de memória a ser editado
 *                  azul: tom de azul
 *                  verde: tom de verde
 *                  vermelho: tom de vermelho
 * retorno ->       0 caso seja bem sucedido ou -1 caso ocorra algum erro
 */
int 
edit_sprite_wsm(int endereco, int azul, int verde, int vermelho) {
    /* Verificações de dados recebidos */
    if (endereco > 12799 || azul > 7 || verde > 7 || vermelho > 7) {
        perror("Valor acima do permitido\n");

        return -1;
    } else if (endereco < 0 || azul < 0 || verde < 0 || vermelho < 0) {
        perror("Valor abaixo do permitido\n");

        return -1;
    }
    
    /* Atribui valores decimais para serem convertidos em string para o buffer */
    data_a = (endereco << 4) | 0b0001;
    data_b = (azul << 6) | (verde << 3) | vermelho;

    /* Tenta enviar dados para o Driver */
    if (preenche_buffer()) {
        perror("Erro ao editar pixel do sprite\n");

        return -1;
    }

    return 0;
}

/**Função para exibir quadrado
 * parâmetros ->    azul: tom de azul
 *                  verde: tom de verde
 *                  vermelho: tom de vermelho
 *                  tamanho: tamanho do quadrado (0 ao 15)
 *                  ref_x: coordenada X do ponto de referência do polígono (0 ao 511)
 *                  ref_y: coordenada Y do ponto de referência do polígono (0 ao 479)
 *                  ordem_impressao: ordem para sobreescrever polígonos (0 ao 15)
 * retorno ->       0 caso seja bem sucedido ou -1 caso ocorra algum erro
 */
int 
set_quadrado_dp(int azul, int verde, int vermelho, int tamanho, int ref_x, int ref_y, int ordem_impressao) {
    /* Verificações de dados recebidos */
    if (azul > 7 || verde > 7 || vermelho > 7 || tamanho > 15 || ref_x > 511 || ref_y > 479 || ordem_impressao > 15) {
        perror("Valor acima do permitido\n");

        return -1;
    } else if (azul < 0 || verde < 0 || vermelho < 0 || tamanho < 0 || ref_x < 0 || ref_y < 0 || ordem_impressao < 0) {
        perror("Valor abaixo do permitido\n");

        return -1;
    }
    
    /* Atribui valores decimais para serem convertidos em string para o buffer */
    data_a = (ordem_impressao << 4) | 0b0011;
    data_b = (0b0 << 31) | (azul << 28) | (verde << 25) | (vermelho << 22) | (tamanho << 18) | (ref_y << 9) | ref_x;

    /* Tenta enviar dados para o Driver */
    if (preenche_buffer()) {
        perror("Erro ao exibir um quadrado\n");

        return -1;
    }

    return 0;
}

/**Função para exibir triangulo
 * parâmetros ->    azul: tom de azul
 *                  verde: tom de verde
 *                  vermelho: tom de vermelho
 *                  tamanho: tamanho do triangulo (0 ao 15)
 *                  ref_x: coordenada X do ponto de referência do polígono (0 ao 511)
 *                  ref_y: coordenada Y do ponto de referência do polígono (0 ao 479)
 *                  ordem_impressao: ordem para sobreescrever polígonos (0 ao 15)
 * retorno ->       0 caso seja bem sucedido ou -1 caso ocorra algum erro
 */
int 
set_triangulo_dp(int azul, int verde, int vermelho, int tamanho, int ref_x, int ref_y, int ordem_impressao) {
    /* Verificações de dados recebidos */
    if (azul > 7 || verde > 7 || vermelho > 7 || tamanho > 15 || ref_x > 511 || ref_y > 480 || ordem_impressao > 15) {
        perror("Valor acima do permitido\n");

        return -1;
    } else if (azul < 0 || verde < 0 || vermelho < 0 || tamanho < 0 || ref_x < 0 || ref_y < 0 || ordem_impressao < 0) {
        perror("Valor abaixo do permitido\n");

        return -1;
    }
    
    /* Atribui valores decimais para serem convertidos em string para o buffer */
    data_a = (ordem_impressao << 4) | 0b0011;
    data_b = (0b1 << 31) | (azul << 28) | (verde << 25) | (vermelho << 22) | (tamanho << 18) | (ref_y << 9) | ref_x;

    /* Tenta enviar dados para o Driver */
    if (preenche_buffer()) {
        perror("Erro ao exibir um triângulo\n");

        return -1;
    }

    return 0;
}

/**Função remover estruturas formadas da tela
 * retorno ->       0 caso seja bem sucedido ou -1 caso ocorra algum erro
 */
int 
limpar_tela() {
    size_t i, j;

    /* Remove cor do background */
    if (set_cor_background_wbr(0, 0, 0) == -1) {
        perror("Erro ao desabilitar cor do background\n");
    }

    /* Remove blocos editados do background */
    for (i = 0; i < 80; ++i) {
        for (j = 0; j < 60; ++j) {
            if (desabilita_bloco_background_wbm(i, j) == -1) {
                perror("Erro ao desabilitar bloco do background\n");

                break;
            }
        }
    }

    /* Remove sprites */
    for (i = 1; i < 32; ++i) {
        if (set_sprite_wbr(0, 0, 0, 0, i) == -1) {
            perror("Erro ao desabilitar sprites\n");

            break; 
        }
    }

    /* Remove Quadrados */
    for (i = 0; i < 16; ++i) {
        if (set_quadrado_dp(0, 0, 0, 0, 0, 0, i) == -1) {
            perror("Erro ao desabilitar quadrados\n");

            break; 
        }
    }

    /* Remove Triângulos */
    for (i = 0; i < 16; ++i) {
        if (set_triangulo_dp(0, 0, 0, 0, 0, 0, i) == -1) {
            perror("Erro ao desabilitar triangulos\n");
            
            break; 
        }
    }

    return 0;
}

/**Função para transformar inteiro em string ( buffer_user = data_b + data_a ) e enviar buffer para o driver
 * retorno ->       0 caso seja bem sucedido ou n bytes caso ocorra algum erro
 */
ssize_t 
preenche_buffer() {
    sprintf(buffer_user, "%010" PRIu32, data_b);                        /* Formata o primeiro número para ocupar 10 caracteres iniciais do buffer */
    
    sprintf(buffer_user + 10, "%010" PRIu32, data_a);                   /* Formata o segundo número para ocupar 10 caracteres finais do buffer */

    lseek(fd, 0, SEEK_SET);                                             /* Lê arquivo do início */

    ssize_t erro = write(fd, buffer_user, sizeof(buffer_user));         /* Verifica se escreveu no driver */

    return erro;
}
