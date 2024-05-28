obj-m += driver.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := /home/aluno/TEC499/TP01/G02

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

