#include "biblioteca_gpu.h"

int main() {

    //Abre arquivo de comunicação com o driver
    if (open_driver( ) == -1){
        return -1;
    }

//-----------------------------------------------------------------------------------------------------------------------//      
    
    //Azul claro (ceu)
    set_cor_background_wbr(6, 4, 3); 

//-----------------------------------------------------------------------------------------------------------------------//

    //Grama verde 1
    edit_bloco_background_wbm(2, 59, 0, 7, 0);
    edit_bloco_background_wbm(1, 59, 0, 7, 0);
    edit_bloco_background_wbm(3, 58, 0, 7, 0);
    edit_bloco_background_wbm(4, 59, 0, 7, 0);
    edit_bloco_background_wbm(4, 58, 0, 7, 0);
    edit_bloco_background_wbm(2, 58, 0, 7, 0);
    edit_bloco_background_wbm(3, 59, 0, 7, 0);
    edit_bloco_background_wbm(5, 59, 0, 7, 0);

    //Grama verde 2
    edit_bloco_background_wbm(77, 59, 0, 7, 0);
    edit_bloco_background_wbm(78, 59, 0, 7, 0);
    edit_bloco_background_wbm(76, 58, 0, 7, 0);
    edit_bloco_background_wbm(75, 59, 0, 7, 0);
    edit_bloco_background_wbm(75, 58, 0, 7, 0);
    edit_bloco_background_wbm(77, 58, 0, 7, 0);
    edit_bloco_background_wbm(76, 59, 0, 7, 0);
    edit_bloco_background_wbm(74, 59, 0, 7, 0);

//-----------------------------------------------------------------------------------------------------------------------//
    
    //Nuvem 1
    edit_bloco_background_wbm(10, 4, 7, 7, 7);
    edit_bloco_background_wbm(11, 4, 7, 7, 7);
    edit_bloco_background_wbm(12, 4, 7, 7, 7);
    edit_bloco_background_wbm(13, 4, 7, 7, 7);
    edit_bloco_background_wbm(10, 5, 7, 7, 7);
    edit_bloco_background_wbm(11, 5, 7, 7, 7);
    edit_bloco_background_wbm(10, 6, 7, 7, 7);

    //Nuvem 2
    edit_bloco_background_wbm(25, 20, 7, 7, 7);
    edit_bloco_background_wbm(26, 20, 7, 7, 7);
    edit_bloco_background_wbm(27, 20, 7, 7, 7);
    edit_bloco_background_wbm(25, 21, 7, 7, 7);
    edit_bloco_background_wbm(26, 21, 7, 7, 7);
    
    //Nuvem 3
    edit_bloco_background_wbm(37, 20, 7, 7, 7);
    edit_bloco_background_wbm(38, 20, 7, 7, 7);
    edit_bloco_background_wbm(39, 20, 7, 7, 7);
    edit_bloco_background_wbm(39, 21, 7, 7, 7);
    edit_bloco_background_wbm(40, 21, 7, 7, 7);
    edit_bloco_background_wbm(40, 22, 7, 7, 7);

    //Nuvem 4
    edit_bloco_background_wbm(58, 17, 7, 7, 7);
    edit_bloco_background_wbm(59, 17, 7, 7, 7);
    edit_bloco_background_wbm(60, 17, 7, 7, 7);
    edit_bloco_background_wbm(59, 18, 7, 7, 7);
    edit_bloco_background_wbm(60, 18, 7, 7, 7);
    edit_bloco_background_wbm(60, 19, 7, 7, 7);

    //Nuvem 5
    edit_bloco_background_wbm(32, 11, 7, 7, 7);
    edit_bloco_background_wbm(33, 11, 7, 7, 7);
    edit_bloco_background_wbm(34, 11, 7, 7, 7);
    edit_bloco_background_wbm(35, 11, 7, 7, 7);
    edit_bloco_background_wbm(32, 10, 7, 7, 7);
    edit_bloco_background_wbm(33, 10, 7, 7, 7);
    edit_bloco_background_wbm(32, 10, 7, 7, 7);

//-----------------------------------------------------------------------------------------------------------------------//

    //Sprite barra
    set_sprite_wbr(1, 320, 445, 11, 1);

    //Sprite alien 1
    set_sprite_wbr(1, 352, 355, 22, 2);

    //Sprite alien 2
    set_sprite_wbr(1, 272, 355, 22, 3);

    //Sprite arvore 1
    set_sprite_wbr(1, 100, 461, 4, 4);

    //Sprite arvore 2
    set_sprite_wbr(1, 511, 461, 4, 5);

    //Sprite tronco 2
    set_sprite_wbr(1, 471, 463, 16, 6);

    //Sprite tronco 1
    set_sprite_wbr(1, 140, 463, 16, 7);

//-----------------------------------------------------------------------------------------------------------------------//

    //Quadrado sol (amarelo)
    set_quadrado_dp(0, 6, 6, 6, 511, 60, 0);

    //Triângulo sol (amarelo)
    set_triangulo_dp(0, 6, 6, 7, 511, 53, 1);

//-----------------------------------------------------------------------------------------------------------------------//
    
    //Quadrado janela 1 (marrom)
    set_quadrado_dp(1, 2, 4, 2, 360, 365, 2);

    //Quadrado janela 2 (marrom)
    set_quadrado_dp(1, 2, 4, 2, 280, 365, 3);

//-----------------------------------------------------------------------------------------------------------------------//

    //Quadrado porta base (marrom)
    set_quadrado_dp(1, 2, 4, 2, 320, 465, 4);

    //Quadrado porta meio (marrom)
    set_quadrado_dp(1, 2, 4, 2, 320, 435, 5);

//-----------------------------------------------------------------------------------------------------------------------//

    //Triângulo teto Casa (marrom)
    set_triangulo_dp(1, 2, 4, 3, 245, 299, 6);

    //Triângulo teto Casa (marrom)
    set_triangulo_dp(1, 2, 4, 3, 275, 299, 7);

    //Triângulo teto Casa (marrom)
    set_triangulo_dp(1, 2, 4, 3, 305, 299, 8);

    //Triângulo teto Casa (marrom)
    set_triangulo_dp(1, 2, 4, 3, 335, 299, 9);

    //Triângulo teto Casa (marrom)
    set_triangulo_dp(1, 2, 4, 3, 365, 299, 10);

    //Triângulo teto Casa (marrom)
    set_triangulo_dp(1, 2, 4, 3, 395, 299, 11);

//-----------------------------------------------------------------------------------------------------------------------//

    //Quadrado parede Casa (branco)
    set_quadrado_dp(7, 7, 7, 15, 320, 400, 12);

//-----------------------------------------------------------------------------------------------------------------------//

    //Remove estruturas da tela
    //limpar_tela();

//-----------------------------------------------------------------------------------------------------------------------//

    //Fecha arquivo de comunicação com o driver
    close_driver();

    return 0;
}
