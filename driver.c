// Bibliotecas inclusas (tem que ver se pode usar essas)
#include <linux/module.h>      // Necessário para todos os módulos do kernel
#include <linux/kernel.h>      // Necessário para KERN_INFO
#include <linux/fs.h>          // Necessário para registrar um dispositivo
#include <linux/uaccess.h>     // Necessário para copy_from_user
#include <linux/io.h>          // Necessário para ioremap e iounmap
#include <linux/cdev.h>        // Necessário para cdev
#include <linux/device.h>      // Necessário para dispositivos de classe

// facilitar os nomes
#define DEVICE_NAME "device"               // Nome do dispositivo
#define CLASS_NAME  "nome_da_classe"       // Nome da classe do dispositivo

//Pegando as definiçoes da biblioteca
#define LW_BRIDGE_BASE  0xFF200000           // Endereço base do LW bridge
#define LW_BRIDGE_SPAN  0x100                // Espaço mapeado do LW bridge
#define DATA_A          0x80                 // Offset para o registrador DATA_A
#define DATA_B          0x70                 // Offset para o registrador DATA_B
#define WRREG           0xc0                 // Offset para o registrador WRREG

// Definições de IOCTL para comandos específicos, não entendi muito bem, mas usa para passar do user space pro kernel space
#define IOCTL_SET_BG_COLOR          _IOW('a', 1, int[3])
#define IOCTL_SET_SPRITE            _IOW('a', 2, int[5])
#define IOCTL_EDIT_BG_BLOCK         _IOW('a', 3, int[4])
#define IOCTL_DISABLE_BG_BLOCK      _IOW('a', 4, int[2])
#define IOCTL_EDIT_SPRITE_PIXEL     _IOW('a', 5, int[4])
#define IOCTL_SET_TRIANGLE          _IOW('a', 6, int[7])
#define IOCTL_SET_SQUARE            _IOW('a', 7, int[7])

static int      major;                          // Número major do dispositivo (Major:minor)
static struct   class  *class = NULL;           // Estrutura da classe do dispositivo
static struct   device *device = NULL;          // Estrutura do dispositivo

// Ponteiros para os registradores mapeados na memória, faltou o WRFULL
static volatile int *DATA_A_PTR;                     // Ponteiro para pegar um DATA_A
static volatile int *DATA_B_PTR;                     // Ponteiro para pegar um DATA_B
static volatile int *WRREG_PTR;                      // Sinal de escrita do buffer de instrução.
//static void          __iomem *LW_virtual;            // Ponteiro para a memória mapeada

// Função para quando o arquivo do dispositivo é aberto
static int dev_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "ABRIU\n");
    return 0;
}

// Função quando é fechado
static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "FECHOU\n");
    return 0;
}

// Função chamada para operações como switch/case, filep pega o arquivo que ta mandando a informação, cmd manda o que fazer
// e arg passa os argumentos pra fazer (tipo cor de background)
static long dev_ioctl(struct file *filep, unsigned int cmd, unsigned long arg) {
    // params aqui guarda a sequencia de inteiros que vai pro kernel
    int params[7];

    // Copiando os dados que vão ser copiados do user space pro kernel space
    if (copy_from_user(params, (int __user *)arg, sizeof(params))) {
        return -EFAULT; // Em caso de erro de acesso a memória
    }

    // Executando os comandos, é bem autoexplicativo
    switch (cmd) {
        case IOCTL_SET_BG_COLOR:
            setCorBackground_WBR(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, params[0], params[1], params[2]);
            break;
        case IOCTL_SET_SPRITE:
            setSprite_WBR(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, params[0], params[1], params[2], params[3], params[4]);
            break;
        case IOCTL_EDIT_BG_BLOCK:
            editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, params[0], params[1], params[2], params[3]);
            break;
        case IOCTL_DISABLE_BG_BLOCK:
            desabilitaBlocoBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, params[0], params[1]);
            break;
        case IOCTL_EDIT_SPRITE_PIXEL:
            editSprite_WSM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, params[0], params[1], params[2], params[3]);
            break;
        case IOCTL_SET_TRIANGLE:
            setTriangulo_DP(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, params[0], params[1], params[2], params[3], params[4], params[5], params[6]);
            break;
        case IOCTL_SET_SQUARE:
            setQuadrado_DP(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, params[0], params[1], params[2], params[3], params[4], params[5], params[6]);
            break;
        default:
            return -EINVAL; // Em caso de argumento inválido
    }

    return 0;
}


//Chamada dsa funções com if/else
/*
static long dev_ioctl(struct file *filep, unsigned int cmd, unsigned long arg) {
    int params[7];
    if (copy_from_user(params, (int __user *)arg, sizeof(params))) {
        return -EFAULT;
    }

    if (cmd == IOCTL_SET_BG_COLOR) {
        setCorBackground_WBR(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, params[0], params[1], params[2]);
    } else if (cmd == IOCTL_SET_SPRITE) {
        setSprite_WBR(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, params[0], params[1], params[2], params[3], params[4]);
    } else if (cmd == IOCTL_EDIT_BG_BLOCK) {
        editBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, params[0], params[1], params[2], params[3]);
    } else if (cmd == IOCTL_DISABLE_BG_BLOCK) {
        desabilitaBlocoBackground_WBM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, params[0], params[1]);
    } else if (cmd == IOCTL_EDIT_SPRITE_PIXEL) {
        editSprite_WSM(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, params[0], params[1], params[2], params[3]);
    } else if (cmd == IOCTL_SET_TRIANGLE) {
        setTriangulo_DP(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, params[0], params[1], params[2], params[3], params[4], params[5], params[6]);
    } else if (cmd == IOCTL_SET_SQUARE) {
        setQuadrado_DP(WRREG_PTR, DATA_A_PTR, DATA_B_PTR, params[0], params[1], params[2], params[3], params[4], params[5], params[6]);
    } else {
        return -EINVAL;
    }

    return 0;
}*/

// Estrutura de operações de arquivo para o kernel fazer a associação
static struct file_operations fops = {
    .open           = dev_open,         // Abrir
    .release        = dev_release,      // Fechar
    .unlocked_ioctl = dev_ioctl,        // Operações de Input/Output
};

// Para iniciar o módulo, aqui ainda ta uma zona...
static int __init device_init(void) {

    // Registrando o dispositivo como um bloco char associado a um major
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "ERRO: Major nao alocado\n");
        return major;
    }

    // Criando a classe do dispositivo
    class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(class)) {
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ALERT "ERRO: Classe do dispositivo nao registrada\n");
        return PTR_ERR(class);
    }

    // Criando o dispositivo
    device = device_create(class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    if (IS_ERR(device)) {
        class_destroy(class);
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ALERT "ERRO: Dispositivo nao foi criado\n");
        return PTR_ERR(device);
    }

    // Aqui seria para mapear a memória, depois eu lembrei que segundo o professor a memória já está fisicamente 
    // alocada e a gente não vai mexer nisso, depois a gente tira
    /*
    LW_virtual = ioremap(LW_BRIDGE_BASE, LW_BRIDGE_SPAN);
    if (!LW_virtual) {
        device_destroy(classe, MKDEV(major, 0));
        class_destroy(classe);
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ALERT "ERRO: mapeamento da memoria\n");
        return -ENOMEM;
    }
    */

    DATA_A_PTR = (int *) (LW_virtual + DATA_A);
    DATA_B_PTR = (int *) (LW_virtual + DATA_B);
    WRREG_PTR  = (int *) (LW_virtual + WRREG);

    printk(KERN_INFO "Dispositivo conectado\n");
    return 0;
}

// Função de saída do módulo
static void __exit device_exit(void) {
    iounmap(LW_virtual);
    device_destroy(classe, MKDEV(major, 0));
    class_unregister(classe);
    class_destroy(classe);
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Dispositivo desconectado\n");
}

module_init(device_init); // Macro que define a função de inicialização do módulo
module_exit(device_exit); // Macro que define a função de saída do módulo

