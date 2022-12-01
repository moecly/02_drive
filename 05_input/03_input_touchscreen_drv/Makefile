KERN_DIR = /linux/bsp/imx-linux4.9.88

all:
	make -C $(KERN_DIR) M=`pwd` modules 
#	$(CROSS_COMPILE)gcc -o button_test button_test.c 

clean:
	make -C $(KERN_DIR) M=`pwd` modules clean
	rm -rf modules.order

obj-m	+= touchscreen.o
