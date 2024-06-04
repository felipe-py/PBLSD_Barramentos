obj-m += driver_tp01_g02.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := /home/aluno/TEC499/TP01/G02

all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean