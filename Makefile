KERNELDIR = /lib/modules/$(shell uname -r)/build
obj-m := bit.o
PWD := $(shell pwd)

all: default rmmod rm insmod mknod main clean

default :
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

rmmod:
	#sudo rmmod bit

rm:
	#sudo rm /dev/bit

insmod:
	sudo insmod bit.ko major=280

mknod:
	sudo mknod -m 666 /dev/bit c 280 0

main:
	gcc -o bit bit.c

clean:
	#rm *.o