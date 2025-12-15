CC := gcc
CFLAGS := -Wall -Wextra -O2


obj-m += net_xmpl.o

all: build_module

build_module:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
