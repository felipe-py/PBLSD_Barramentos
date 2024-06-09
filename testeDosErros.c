#include "biblioteca_gpu.h"

int main() {

    //Abre arquivo de comunicação com o driver
    open_driver();

    //Cor acima do permitido
    set_cor_background_wbr(8, 4, 3); 

    //Offset acima do permitido
    //set_sprite_wbr(1, 320, 445, 32, 1);

    //Ref_x e Ref_y abaixo do permitido
    //set_quadrado_dp(0, 6, 6, 1, 9, 9, 0);

    //Fecha arquivo de comunicação com o driver
    close_driver();

    return 0;
}
