#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <asm/io.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>

/* Metadados do driver */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lucas Gabriel, Luis Felipe, Lucas Lima");
MODULE_DESCRIPTION("Driver para comunicação com uma GPU");
MODULE_VERSION("1.0");

#define DEVICE_NAME "driver_tp01_g02"       /* Nome do dispositivo */

/* Definindo endereços dos barramentos e sinais */
#define LW_BRIDGE_BASE  0xFF200000           /* Endereço base do LW bridge */
#define LW_BRIDGE_SPAN  0x100                /* Espaço mapeado do LW bridge */
#define DATA_A          0x80                 /* Offset para o registrador DATA_A */
#define DATA_B          0x70                 /* Offset para o registrador DATA_B */
#define WRREG           0xc0                 /* Offset para o registrador WRREG */
#define WRFULL          0xb0                 /* Offser para o registrador WRFULL */

/* Estrutura para identificar e acessar dispositivos dentro do SO */
static struct {
    dev_t devnum;                           /* Número do dispositivo */
    struct cdev cdev;                       /* Dispositivo de caractere */
} dev_data;

static void *lw_virtual;                    /* Endereço da ponte light-weight */

static volatile int *data_a_ptr;            /* Endereço para Registrador DATAA */
static volatile int *data_b_ptr;            /* Endereço para Registrador DATAB */
static volatile int *wrreg_ptr;             /* Endereço para Registrador WRREG */
static volatile int *wrfull_ptr;            /* Endereço para Registrador WRFULL */

static ssize_t ret;                         /* Bytes não copiados */

static char buffer_nucleo[21];              /* Buffer do driver para enviar ao Usuário*/

/**Função para quando o arquivo do dispositivo é aberto
 * parâmetros ->    inodep: ponteiro para informações sobre o arquivo em nível de sistema de arquivos
 *                  filep: ponteiro para informações específicas sobre o arquivo que está sendo manipulado durante a abertura
 */
static int dev_open(struct inode* inodep, struct file* filep) {
    pr_info("%s: abriu!\n", DEVICE_NAME);

    return 0;
}

/**Função para quando o arquivo do dispositivo é fechado
 * parâmetros ->    inodep: ponteiro para informações sobre o arquivo em nível de sistema de arquivos
 *                  filep: ponteiro para informações específicas sobre o arquivo que está sendo manipulado durante o fechamento
 */
static int dev_close(struct inode* inodep, struct file* filep) {
    pr_info("%s: fechou!\n", DEVICE_NAME);

    return 0;
}

/**Função para ler arquivo do dispositivo
 * parâmetros ->    file: ponteiro para informações sobre o arquivo que está sendo lido
 *                  buffer_user: ponteiro para o buffer no espaço de usuário onde os dados lidos devem ser armazenados
 *                  buffer_bytes: número de bytes a serem lidos do dispositivo e copiados para o buffer do espaço de usuário
 *                  curs: ponteiro para a posição de leitura no arquivo
 * retorno ->       0 caso seja bem sucedido ou n bytes caso ocorra algum erro 
 */
static ssize_t dev_read(struct file* file, char* buffer_user, size_t buffer_bytes, loff_t* curs) {
    pr_info("%s: lendo!\n", DEVICE_NAME);
    
    ret = copy_to_user(buffer_user, buffer_nucleo, buffer_bytes);       /* Do Kernel para usuário */

    /* Caso ret seja diferente de 0, retorna bytes que não foram copiados */
    if (ret) {
        pr_err("%s: falha ao copiar buffer do núcleo para buffer do usuário\n", DEVICE_NAME);

        return ret;
    }

    pr_info("%s: leitura feita com sucesso!\n", DEVICE_NAME);

    return 0;
}

/**Função para escrever no arquivo do dispositivo
 * parâmetros ->    file: ponteiro para informações sobre o arquivo que está sendo escrito
 *                  buffer_user: ponteiro para o buffer no espaço de usuário que contém os dados a serem escritos no dispositivo
 *                  buffer_bytes: número de bytes a serem lidos do buffer do espaço de usuário e escritos no dispositivo
 *                  curs: ponteiro para a posição de escrita no arquivo
 *                  data_a: variável para guardar valor inteiro convertido do buffer, dados a serem enviados ao registrador DATA_A
 *                  data_b: variável para guardar valor inteiro convertido do buffer, dados a serem enviados ao registrador DATA_B
 * retorno ->       0 caso seja bem sucedido ou n bytes caso ocorra algum erro 
 */
static ssize_t dev_write(struct file* file, const char* buffer_user, size_t buffer_bytes, loff_t* curs) {
    pr_info("%s: escrevendo!\n", DEVICE_NAME);

    /* Enquanto FIFO's estiverem cheias, espera */
    while (*wrfull_ptr) {}
    
    ret = copy_from_user(buffer_nucleo, buffer_user, buffer_bytes);         /* Do usuário para kernel */

    /* Caso ret seja diferente de 0, retorna bytes que não foram copiados */
    if (ret) {
        pr_err("%s: falha ao copiar buffer do usuário para buffer do núcleo\n", DEVICE_NAME);

        return ret;
    }

    /* Inteiros para DATA_A e DATA_B */
    uint32_t data_a = 0, data_b = 0;

    int i = 0;

    /* Converte caractere para número */
    for (i = 0; i < 20; ++i) {
        if (i < 10) {       /* Pega 10 primeiros caracteres do buffer e converte para número */
            data_b = data_b * 10 + (buffer_nucleo[i] - '0');   
        } else {            /* Pega os outros 10 caracteres do buffer e converte para número */
            data_a = data_a * 10 + (buffer_nucleo[i] - '0');
        }
    }

    /* Envia instruções para as FIFO's */
    *data_a_ptr = data_a;
    *data_b_ptr = data_b;

    /* Envia sinal para escrita na fila */
    *wrreg_ptr = 1;
    *wrreg_ptr = 0;

    pr_info("%s: escrita feita com sucesso!\n", DEVICE_NAME);

    return 0;
}

/* Operações que podem ser realizadas */
static const struct file_operations fops = {
    .owner = THIS_MODULE,       /* Módulo proprietário das funções (o próprio) */
    .open = dev_open,           /* Abre dispositivo */
    .release = dev_close,       /* Fecha dispositivo */
    .read = dev_read,           /* Lê dados do dispositivo */
    .write = dev_write          /* Escrever dados no dispositivo */
};

/**Função para inicializar o módulo
 * parâmetros ->    result: variável para guardar retorno de algumas operações
 * retorno ->       0 caso seja bem sucedido ou -1 caso ocorra algum erro 
 */
static int __init dev_init(void) {
    int result;

    result = alloc_chrdev_region(&dev_data.devnum, 0, 1, DEVICE_NAME);      /* Kernel aloca dinamicamente número de dispositivo disponível (major) */

    /* Se for diferente de 0, deu erro */
    if (result) {
        pr_err("%s: erro ao alocar número do dispositivo!\n", DEVICE_NAME);

        return result;
    }

    cdev_init(&dev_data.cdev, &fops);                               /* Inicializa a estrutura cdev */

    result = cdev_add(&dev_data.cdev, dev_data.devnum, 1);          /* Registra dispositivo de caractere no kernel do Linux e associa seu número */

    /* Se for diferente de 0, exclui registro de número de dispositivo alocado */
    if (result) {
        pr_err("%s: erro no registro do dispositivo char!\n", DEVICE_NAME);
        unregister_chrdev_region(dev_data.devnum, 1);

        return result;
    }

    lw_virtual = ioremap_nocache(LW_BRIDGE_BASE, LW_BRIDGE_SPAN);   /* Mapeia ponte lightweight */
    
    /* Mapeia barramentos e sinais */
    data_a_ptr = (int*) (lw_virtual + DATA_A);
    data_b_ptr = (int*) (lw_virtual + DATA_B);
    wrreg_ptr = (int*) (lw_virtual + WRREG);
    wrfull_ptr = (int*) (lw_virtual + WRFULL);

    pr_info("%s: inicializado!\n", DEVICE_NAME);

    return 0;
}

/* Função para encerrar o módulo */
static void __exit dev_exit(void) {
    iounmap(lw_virtual);                                    /* Desfaz mapeamento */

    cdev_del(&dev_data.cdev);                               /* Exclui registro do dispositivo */

    unregister_chrdev_region(dev_data.devnum, 1);           /* Exclui registro de número de dispositivo alocado */

    pr_info("%s: encerrado!\n", DEVICE_NAME);
}

module_init(dev_init); /* Macro que define a função de inicialização do módulo */
module_exit(dev_exit); /* Macro que define a função de saída do módulo */
