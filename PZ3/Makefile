CC := gcc
CFLAGS := -Wall -Wextra -O2


obj-m += prikol.o

all: build_module build_user

build_module:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

build_user:
	$(CC) $(CFLAGS) -o user_app user_app.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
