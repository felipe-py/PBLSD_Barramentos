#ifndef BIBLIOTECA_GPU_H
#define BIBLIOTECA_GPU_H

/* Caminho do Driver */
#define CAMINHO_DRIVER "/dev/driver_tp01_g02"

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
edit_background_wbm(int bloco_x, int bloco_y, int azul, int verde, int vermelho);

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

/* Função remover estruturas formadas da tela */
int 
limpar_tela();

/* Função para transformar inteiro em string ( buffer_user = data_b + data_a ) e enviar buffer para o driver */
int 
preenche_buffer();

#endif
