#include "biblioteca_gpu.h"
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

static uint32_t data_a, data_b;     /* Inteiros que representam dados a serem enviados aos registradores DATA_A e DATA_B */

static int fd;                      /* Descritor */

static char buffer_user[21];        /* Buffer do usuário para enviar ao Driver */

static int blocos_editados_x[4802];     /* Armazena blocos_x editados */

static int blocos_editados_y[4802];     /* Armazena blocos_y editados */

static int registrador_poligonos_setados[18];       /* Armazena registradores dos Poligonos usados */

static int registrador_sprites_setados[33];       /* Armazena registradores dos Sprites usados */

static int qtd_blocos_editados = 0;     /* Quantidade de Blocos editados */

static int qtd_sprites_setados = 0;     /* Quantidade de Sprites editados */

static int qtd_poligonos_setados = 0;       /* Quantidade de Poligonos editados */

/**Função para inicializar comunicação com o driver
 * retorno ->       Retorna 0 caso comunicação seja iniciada ou -1 caso dê erro
 */
int 
open_driver() {
    /* Abre o arquivo com permissão de escrita e leitura.*/
    fd = open(CAMINHO_DRIVER, (O_RDWR | O_SYNC));

    if (fd == -1) {
        fprintf(stderr, "Falha ao abrir o arquivo\n");
        return -1;
    }

    return 0;
}

/**Função para encerrar comunicação com o driver
 * retorno ->       Retorna 0 caso comunicação seja encerrada ou -1 caso dê erro
 */
int 
close_driver() {
    /* Fecha o arquivo */
    if (close(fd) == -1) {
        fprintf(stderr, "Falha ao encerrar o arquivo\n");
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
        fprintf(stderr, ERRO_SET_COR_BACKGROUND"Valor acima do permitido\n");
        return -1;
    } else if (azul < 0 || verde < 0 || vermelho < 0) {
        fprintf(stderr, ERRO_SET_COR_BACKGROUND"Valor abaixo do permitido\n");
        return -1;
    }
    
    /* Atribui valores decimais para serem convertidos em char para o buffer */
    data_a = 0b0000;
    data_b = (azul << 6) | (verde << 3) | vermelho;

    /* Tenta enviar dados para o Driver */
    if (preenche_buffer()) {
        fprintf(stderr, ERRO_SET_COR_BACKGROUND"Falha na escrita\n");
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
    size_t i;
    
    /* Verificações de dados recebidos */
    if (ativa_sprite > 1 || cord_x > 639 || cord_y > 479 || offset > 31 || registrador > 31) {
        fprintf(stderr, ERRO_SET_SPRITE"Valor acima do permitido\n");
        return -1;
    } else if (ativa_sprite < 0 || cord_x < 0 || cord_y < 0 || offset < 0 || registrador < 1) {
        fprintf(stderr, ERRO_SET_SPRITE"Valor abaixo do permitido\n");
        return -1;
    }

    /* Atribui valores decimais para serem convertidos em char para o buffer */  
    data_a = (registrador << 4) | 0b0000;
    data_b = (ativa_sprite << 29) | (cord_x << 19) | (cord_y << 9) | offset;

    /* Tenta enviar dados para o Driver */
    if (preenche_buffer()) {
        fprintf(stderr, ERRO_SET_SPRITE"Falha na escrita\n");
        return -1;
    }

    /* Verifica se registrador já foi utilzado */
    if(qtd_sprites_setados != 0) {
        for(i = 0; i < qtd_sprites_setados; ++i) {
            if(registrador_sprites_setados[i] == registrador) {
                return 0;
            }
        }
    }

    /* Se não foi, armazena registrador utilizado e contabiliza */
    registrador_sprites_setados[qtd_sprites_setados] = registrador;
    qtd_sprites_setados++;

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
edit_bloco_background_wbm(int bloco_x, int bloco_y, int azul, int verde, int vermelho) {
    size_t i;
    int bloco;

    bloco = bloco_y * 80 + bloco_x;     /* Multiplica linha recebida pelo total de colunas (80) e soma à coluna recebida */
    
    /* Verificações de dados recebidos */
    if (bloco > 4799 || azul > 7 || verde > 7 || vermelho > 7) {
        fprintf(stderr, ERRO_EDIT_BLOCO_BACKGROUND"Valor acima do permitido\n");
        return -1;
    } else if (bloco < 0 || azul < 0 || verde < 0 || vermelho < 0) {
        fprintf(stderr, ERRO_EDIT_BLOCO_BACKGROUND"Valor abaixo do permitido\n");
        return -1;
    }
    
    /* Atribui valores decimais para serem convertidos em char para o buffer */
    data_a = (bloco << 4) | 0b0010;
    data_b = (azul << 6) | (verde << 3) | vermelho;

    /* Tenta enviar dados para o Driver */
    if (preenche_buffer()) {
        fprintf(stderr, ERRO_EDIT_BLOCO_BACKGROUND"Falha na escrita\n");
        return -1;
    }

    /* Verifica se bloco x e y já foram utilzados */
    if(qtd_blocos_editados != 0) {
        for(i = 0; i < qtd_blocos_editados; ++i) {
            if(blocos_editados_x[i] == bloco_x && blocos_editados_y[i] == bloco_y) {
                return 0;
            }
        }
    }

    /* Se não foi, armazena blocos x e y utilizados e contabiliza */
    blocos_editados_x[qtd_blocos_editados] = bloco_x;
    blocos_editados_y[qtd_blocos_editados] = bloco_y;
    qtd_blocos_editados++;

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
    int bloco;

    bloco = bloco_y * 80 + bloco_x;     /* Multiplica linha recebida pelo total de colunas (80) e soma à coluna recebida */
    
    /* Verificações de dados recebidos */
    if (bloco > 4799) {
        fprintf(stderr, ERRO_DESABILITA_BLOCO_BACKGROUND"Valor acima do permitido\n");
        return -1;
    } else if (bloco < 0) {
        fprintf(stderr, ERRO_DESABILITA_BLOCO_BACKGROUND"Valor abaixo do permitido\n");
        return -1;
    }

    /* Atribui valores decimais para serem convertidos em char para o buffer */
    data_a = (bloco << 4) | 0b0010;
    data_b = 0b111111110;

    /* Tenta enviar dados para o Driver */
    if (preenche_buffer()) {
        fprintf(stderr, ERRO_DESABILITA_BLOCO_BACKGROUND"Falha na escrita\n");
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
        fprintf(stderr, ERRO_EDIT_SPRITE"Valor acima do permitido\n");
        return -1;
    } else if (endereco < 0 || azul < 0 || verde < 0 || vermelho < 0) {
        fprintf(stderr, ERRO_EDIT_SPRITE"Valor abaixo do permitido\n");
        return -1;
    }
    
    /* Atribui valores decimais para serem convertidos em char para o buffer */
    data_a = (endereco << 4) | 0b0001;
    data_b = (azul << 6) | (verde << 3) | vermelho;

    /* Tenta enviar dados para o Driver */
    if (preenche_buffer()) {
        fprintf(stderr, ERRO_EDIT_SPRITE"Falha na escrita\n");
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
 *                  limite: variável usada para verificar se ref_x e ref_y não ultrapassam 
 *                          limite da tela a depender do tamanho do polígono (tamanho 0 não é verificado)
 * retorno ->       0 caso seja bem sucedido ou -1 caso ocorra algum erro
 */
int 
set_quadrado_dp(int azul, int verde, int vermelho, int tamanho, int ref_x, int ref_y, int ordem_impressao) { 
    size_t limite, i;

    /* Verifica se coordenadas x e y estão dentro do limite da tela permitido a depender do tamanho escolhido */
    limite = 5 * (tamanho - 1) + 9;
    
    /* Verificações de dados recebidos */
    if (azul > 7 || verde > 7 || vermelho > 7 || tamanho > 15 || ref_x > 511 || ref_y > 479 || ordem_impressao > 15) {
        fprintf(stderr, ERRO_SET_QUADRADO"Valor acima do permitido\n");
        return -1;
    } else if (azul < 0 || verde < 0 || vermelho < 0 || tamanho < 0 || ref_x < 0 || ref_y < 0 || ordem_impressao < 0) {
        fprintf(stderr, ERRO_SET_QUADRADO"Valor abaixo do permitido\n");
        return -1;
    } else if (tamanho > 0){ 
        if (ref_x <= limite || ref_y <= limite) {
            fprintf(stderr, ERRO_SET_QUADRADO"Valor da coordenada x ou y abaixo do limite permitido pelo tamanho\n");
            return -1;
        }
    }
    
    /* Atribui valores decimais para serem convertidos em char para o buffer */
    data_a = (ordem_impressao << 4) | 0b0011;
    data_b = (0b0 << 31) | (azul << 28) | (verde << 25) | (vermelho << 22) | (tamanho << 18) | (ref_y << 9) | ref_x;

    /* Tenta enviar dados para o Driver */
    if (preenche_buffer()) {
        fprintf(stderr, ERRO_SET_QUADRADO"Falha na escrita\n");
        return -1;
    }

    /* Verifica se registrador já foi utilzado */
    if(qtd_poligonos_setados != 0) {
        for(i = 0; i < qtd_poligonos_setados; ++i) {
            if(registrador_poligonos_setados[i] == ordem_impressao) {
                return 0;
            }
        }
    }

    /* Se não foi, armazena registrador utilizado e contabiliza */
    registrador_poligonos_setados[qtd_poligonos_setados] = ordem_impressao;
    qtd_poligonos_setados++;

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
 *                  limite: variável usada para verificar se ref_x e ref_y não ultrapassam 
 *                          limite da tela a depender do tamanho do polígono (tamanho 0 não é verificado)
 * retorno ->       0 caso seja bem sucedido ou -1 caso ocorra algum erro
 */
int 
set_triangulo_dp(int azul, int verde, int vermelho, int tamanho, int ref_x, int ref_y, int ordem_impressao) {
    size_t limite, i;

    /* Verifica se coordenadas x e y estão dentro do limite da tela permitido a depender do tamanho escolhido */
    limite = 5 * (tamanho - 1) + 9;

    /* Verificações de dados recebidos */
    if (azul > 7 || verde > 7 || vermelho > 7 || tamanho > 15 || ref_x > 511 || ref_y > 480 || ordem_impressao > 15) {
        fprintf(stderr, ERRO_SET_TRIANGULO"Valor acima do permitido\n");
        return -1;
    } else if (azul < 0 || verde < 0 || vermelho < 0 || tamanho < 0 || ref_x < 0 || ref_y < 0 || ordem_impressao < 0) {
        fprintf(stderr, ERRO_SET_TRIANGULO"Valor abaixo do permitido\n");
        return -1;
    } else if (tamanho > 0){ 
        if (ref_x <= limite || ref_y <= limite) {
            fprintf(stderr, ERRO_SET_QUADRADO"Valor da coordenada x ou y abaixo do limite permitido pelo tamanho\n");
            return -1;
        }
    }
    
    /* Atribui valores decimais para serem convertidos em char para o buffer */
    data_a = (ordem_impressao << 4) | 0b0011;
    data_b = (0b1 << 31) | (azul << 28) | (verde << 25) | (vermelho << 22) | (tamanho << 18) | (ref_y << 9) | ref_x;

    /* Tenta enviar dados para o Driver */
    if (preenche_buffer()) {
        fprintf(stderr, ERRO_SET_TRIANGULO"Falha na escrita\n");
        return -1;
    }

    /* Verifica se registrador já foi utilzado */
    if(qtd_poligonos_setados != 0) {
        for(i = 0; i < qtd_poligonos_setados; ++i) {
            if(registrador_poligonos_setados[i] == ordem_impressao) {
                return 0;
            }
        }
    }

    /* Se não foi, armazena registrador utilizado e contabiliza */
    registrador_poligonos_setados[qtd_poligonos_setados] = ordem_impressao;
    qtd_poligonos_setados++;

    return 0;
}

/**Função remover estruturas formadas na tela
 * retorno ->       0 caso seja bem sucedido ou -1 caso ocorra algum erro
 */
int 
limpar_tela() {
    size_t i;

    /* Remove cor do background */
    set_cor_background_wbr(0, 0, 0);

    /* Remove apenas blocos editados do background */
    for (i = 0; i < qtd_blocos_editados; ++i) {
        if (desabilita_bloco_background_wbm(blocos_editados_x[i], blocos_editados_y[i]) == -1) {
            break;
        }
    }

    /* Remove apenas Sprites utilizados */
    for (i = 0; i < qtd_sprites_setados; ++i) {
        if (set_sprite_wbr(0, 0, 0, 0, registrador_sprites_setados[i]) == -1) {
            break; 
        }
    }

    /* Remove apenas Poligonos utilizados */
    for (i = 0; i < qtd_poligonos_setados; ++i) {
        if (set_quadrado_dp(0, 0, 0, 0, 0, 0, registrador_poligonos_setados[i]) == -1) {
            break; 
        }
    }

    /* Zera contadores de elementos utilizados */
    qtd_blocos_editados = 0;
    qtd_poligonos_setados = 0;
    qtd_sprites_setados = 0;

    return 0;
}

/**Função para transformar inteiro em char ( buffer_user = data_b + data_a ) e enviar buffer para o driver
 *                  erro: variável usada para retorno da operação de escrita no driver
 * retorno ->       0 caso seja bem sucedido ou n bytes caso ocorra algum erro
 */
ssize_t 
preenche_buffer() {
    ssize_t erro;

    sprintf(buffer_user, "%010" PRIu32, data_b);                        /* Formata o primeiro número para ocupar 10 caracteres iniciais do buffer */
    
    sprintf(buffer_user + 10, "%010" PRIu32, data_a);                   /* Formata o segundo número para ocupar 10 caracteres finais do buffer */

    lseek(fd, 0, SEEK_SET);                                             /* Lê arquivo do início */

    erro = write(fd, buffer_user, sizeof(buffer_user));                 /* Verifica se escreveu no driver */

    return erro;
}
