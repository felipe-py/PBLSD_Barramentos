#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <asm/io.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h> // Para usar uint32_t

//Pegando as definições da biblioteca
#define LW_BRIDGE_BASE  0xFF200000           // Endereço base do LW bridge
#define LW_BRIDGE_SPAN  0x100                // Espaço mapeado do LW bridge
#define DATA_A          0x80                 // Offset para o registrador DATA_A
#define DATA_B          0x70                 // Offset para o registrador DATA_B
#define WRREG           0xc0                 // Offset para o registrador WRREG
#define WRFULL          0xb0                 // Offser para o registrador WRFULL

#define DEVICE_NAME "driver_tp01_g02"        // Nome do dispositivo

static struct {
	dev_t devnum;           //Número do dispositivo
	struct cdev cdev;       //Dispositivo de caractere   
} dev_data;

static void *LW_virtual;    //Endereço da ponte light-weight

static volatile int *DATA_A_PTR;    //Endereço para Registrador DATAA
static volatile int *DATA_B_PTR;    //Endereço para Registrador DATAB   
static volatile int *WRREG_PTR;     //Endereço para Registrador WRREG
static volatile int *WRFULL_PTR;    //Endereço para Registrador WRFULL

static char buffer_nucleo[21];      //Buffer do driver
static ssize_t ret;                 //Indica sucesso ou falha ao copiar buffers

//Função para quando o arquivo do dispositivo é aberto
static int dev_open(struct inode *inodep, struct file *filep) {
    pr_info("%s: abriu!\n", DEVICE_NAME);
    return 0;
}

//Função para quando o arquivo do dispositivo é fechado
static int dev_close(struct inode *inodep, struct file *filep) {
    pr_info("%s: fechou!\n", DEVICE_NAME);
    return 0;
}

//Função para ler arquivo do dispositivo

//Arquivo
//Buffer de dados no espaço do usuário onde os dados lidos devem ser armazenados
//Número de bytes a serem lidos
static ssize_t dev_read(struct file* file, char* buffer_user, size_t buffer_bytes, loff_t *curs){
    pr_info("%s: lendo!\n", DEVICE_NAME);
    
    //Do Kernel para usuário
    ret = copy_to_user(buffer_user, buffer_nucleo, buffer_bytes);

    //Caso ret seja diferente de 0, retorna bytes que não foram copiados
    if(ret) {
        pr_err("%s: falha ao copiar buffer do núcleo para buffer do usuário\n", DEVICE_NAME);
        return ret;
    }

    pr_info("%s: leitura feita com sucesso!\n", DEVICE_NAME);

    return 0;
}

//Função para escrever no arquivo do dispositivo

//Arquivo
//Buffer de dados do espaço do usuário a ser armazenado
//Número de bytes a serem lidos
static ssize_t dev_write(struct file *file, const char *buffer_user, size_t buffer_bytes, loff_t *curs){
    pr_info("%s: escrevendo!\n", DEVICE_NAME);
    
    //Do usuário para kernel
    ret = copy_from_user(buffer_nucleo, buffer_user, buffer_bytes);

    //Caso ret seja diferente de 0, retorna bytes que não foram copiados
    if(ret) {
        pr_err("%s: falha ao copiar buffer do usuário para buffer do núcleo\n", DEVICE_NAME);
        return ret;
    }

    //Inteiros para DATA_A e DATA_B
    uint32_t dataA = 0;
    uint32_t dataB = 0;

    int i = 0;

    while (i < 20) {
        //Pega 10 primeiros números do buffer
        if (i < 10) {
            dataB = dataB * 10 + (buffer_nucleo[i] - '0');
        } 
        
        //Pega os outros 10 números do buffer
        else {
            dataA = dataA * 10 + (buffer_nucleo[i] - '0');
        }

        i++;
    }
    
    printk("%u\n", dataA);
    printk("%u\n", dataB);

    //Enquanto FIFO's estiverem cheias, espera
    while (*WRFULL_PTR) {}

    //Envia instruções para as FIFO's
    *DATA_A_PTR = dataA;
    *DATA_B_PTR = dataB;

    //Envia sinal para escrita na fila
    *WRREG_PTR = 1;
    *WRFULL_PTR = 0;

    pr_info("%s: escrita feita com sucesso!\n", DEVICE_NAME);

    return 0;
}

//Operações que podem ser realizadas
static const struct file_operations fops = {
    .owner = THIS_MODULE,       //Módulo proprietário das funções (o próprio)
    .open = dev_open,           //Abre dispositivo
    .release = dev_close,       //Fecha dispositivo
    .read = dev_read,           //Lê dados do dispositivo
    .write = dev_write          //Escrever dados no dispositivo
};

//Função para inicializar o módulo
static int __init dev_init(void){
    int result;

    //Aloca número do dispositivo
    result = alloc_chrdev_region(&dev_data.devnum, 0, 1, DEVICE_NAME);

    //Se for diferente de 0, deu erro
    if(result) {
        pr_err("%s: erro ao alocar número do dispositivo!\n", DEVICE_NAME);
        return result;
    }

    //Inicializa a estrutura cdev
    cdev_init(&dev_data.cdev, &fops);

    //Registra dispositivo de caractere no kernel do Linux e associa seu número
    result = cdev_add(&dev_data.cdev, dev_data.devnum, 1);

    //Se for diferente de 0, deu erro
    if (result) {
        pr_err("%s: erro no registro do dispositivo char!\n", DEVICE_NAME);
        unregister_chrdev_region(dev_data.devnum, 1);
        return result;
    }

    //Mapeia lightweight HPS-to-FPGA brigde
    LW_virtual = ioremap(LW_BRIDGE_BASE, LW_BRIDGE_SPAN);
    
    //Mapeia barramentos e sinais
    DATA_A_PTR = (int*) (LW_virtual + DATA_A);
    DATA_B_PTR = (int*) (LW_virtual + DATA_B);
    WRREG_PTR = (int*) (LW_virtual + WRREG);
    WRFULL_PTR = (int*) (LW_virtual + WRFULL);

    pr_info("%s: inicializado!\n", DEVICE_NAME);

    return 0;
}

//Função para encerrar o módulo
static void __exit dev_exit(void){
    //Desfaz mapeamento
    iounmap(LW_virtual);

    //Exclui registro do dispositivo
    cdev_del(&dev_data.cdev);

    //Exclui registro de número de dispositivo alocado
    unregister_chrdev_region(dev_data.devnum, 1);

    pr_info("%s: encerrado!\n", DEVICE_NAME);
}

module_init(dev_init); //Macro que define a função de inicialização do módulo
module_exit(dev_exit); //Macro que define a função de saída do módulo
