# driver_tp01_g02.o deve ser incluído na lista de módulos a serem compilados
obj-m += driver_tp01_g02.o

# Diretório onde estão os headers e makefiles do kernel
KDIR := /lib/modules/$(shell uname -r)/build

# Diretório atual do projeto
PWD := /home/aluno/TEC499/TP01/G02

# Regra padrão para compilar módulo kernel, inserí-lo no kernel dinamicamente e criar um nó de dispositivo de caractere (arquivo especial)
all:
	make -C $(KDIR) M=$(PWD) modules

	insmod driver_tp01_g02.ko

	mknod /dev/driver_tp01_g02 c 241 0

# Regra para limpar os arquivos gerados de compilação do módulo, remover módulo do kernel e seu nó de dispositivo de caractere (arquivo especial)
clean:
	make -C $(KDIR) M=$(PWD) clean

	rmmod driver_tp01_g02.ko

	rm -f /dev/driver_tp01_g02

# Regra para compilar e executar programa de usuário
run:
	gcc main.c biblioteca_gpu.c -std=c99 -o programa

	./programa

# Regra para remover todos os arquivos do diretório
clearAll:
	rm *
