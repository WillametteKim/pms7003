KERNELDIR = /lib/modules/$(shell uname -r)/build
obj-m := gpio.o
PWD := $(shell pwd)


all: default rmmod rm insmod mknod main clean

default :
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

rmmod:
	#sudo rmmod main_gpio

rm:
	#sudo rm /dev/main_gpio

insmod:
	sudo insmod gpio.ko major=280

mknod:
	sudo mknod -m 666 /dev/gpio c 280 0

main:
	gcc -o gpio gpio.c

clean:
	rm *.o
