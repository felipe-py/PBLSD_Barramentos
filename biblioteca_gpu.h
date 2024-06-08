#ifndef BIBLIOTECA_GPU_H
#define BIBLIOTECA_GPU_H

/* Caminho do Driver */
#define CAMINHO_DRIVER "/dev/driver_tp01_g02"

/* Erros associados a cada função */
#define ERRO_SET_COR_BACKGROUND "Erro ao mudar cor base do background: "
#define ERRO_SET_SPRITE "Erro ao exibir sprite: "
#define ERRO_EDIT_BLOCO_BACKGROUND "Erro ao editar bloco do background: "
#define ERRO_DESABILITA_BLOCO_BACKGROUND "Erro ao desabilitar bloco do background: "
#define ERRO_EDIT_SPRITE "Erro ao editar pixel do sprite: "
#define ERRO_SET_QUADRADO "Erro ao exibir um quadrado: "
#define ERRO_SET_TRIANGULO "Erro ao exibir um triângulo: "

/* Função para inicializar comunicação com o driver */
int 
open_driver();

/* Função para encerrer comunicação com o driver */
int 
close_driver();

/* Função para mudar a cor do background */
int 
set_cor_background_wbr(int azul, int verde, int vermelho);

/* Função para exibir sprite */
int 
set_sprite_wbr(int ativaSprite, int cord_x, int cord_y, int offset, int registrador);

/* Função para editar um bloco do background */
int 
edit_bloco_background_wbm(int bloco_x, int bloco_y, int azul, int verde, int vermelho);

/* Função para desabilitar um bloco do background */
int 
desabilita_bloco_background_wbm(int bloco_x, int bloco_y);

/* Função para editar um pixel de um sprite armazenado ou criar um pixel de um novo sprite */
int 
edit_sprite_wsm(int endereco, int azul, int verde, int vermelho);

/* Função para exibir quadrado */
int 
set_quadrado_dp(int azul, int verde, int vermelho, int tamanho, int ref_x, int ref_y, int ordem_impressao);

/* Função para exibir triangulo */
int 
set_triangulo_dp(int azul, int verde, int vermelho, int tamanho, int ref_x, int ref_y, int ordem_impressao);

/* Função remover estruturas formadas na tela */
int 
limpar_tela();

/* Função para transformar inteiro em char ( buffer_user = data_b + data_a ) e enviar buffer para o driver */
int 
preenche_buffer();

#endif
