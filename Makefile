obj-m += driver_tp01_g02.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := /home/aluno/TEC499/TP01/G02

all:
	make -C $(KDIR) M=$(PWD) modules

	insmod driver_tp01_g02.ko

	mknod /dev/driver_tp01_g02 c 241 0
	
clean:
	make -C $(KDIR) M=$(PWD) clean

	rmmod driver_tp01_g02.ko

	rm -f /dev/driver_tp01_g02

run:
	gcc main.c biblioteca_gpu.c -std=c99 -o programa

	./programa
