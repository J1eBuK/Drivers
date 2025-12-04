# Задача №1

Собрать модуль ядра. Переименовывем `Makefile.mk` в `Makefile` и запускаем make.

```
dudka@DVM:~/Drivers/PZ2$ cp Makefile.mk Makefile dudka@DVM:~/Drivers/PZ2$ make
make -C /lib/modules/6.14.0-36-generic/build/ M=/home/dudka/Drivers/PZ2 modules make[1]: Entering directory '/usr/src/linux-headers-6.14.0-36-generic'
make[2]: Entering directory '/home/dudka/Drivers/PZ2'
warning: the compiler differs from the one used to build the kernel
The kernel was built by: x86_64-linux-gnu-gcc-13 (Ubuntu 13.3.0-6ubuntu2 24.04) 13.3.0 gcc-13 (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
You are using:
CC [M] simple.o
MODPOST Module.symvers
CC [M]
CC [M]
LD [M]
simple.mod.o
.module-common.o
simple.ko
BTF [M] simple.ko
Skipping BTF generation for simple.ko due to unavailability of vmlinux make[2]: Leaving directory /home/dudka/Drivers/PZ2'
make[1]: Leaving directory '/usr/src/linux-headers-6.14.0-36-generic' dudka@DVM:~/Drivers/PZ2$
```
Готово, модуль собран.

# Задача №2

Вставить модуль ядра и проверить.

Вызовем команду `sudo insmod simple.ko`.

Проверим что модуль загружен командой `lsmod | grep simple`.

Получаем строку:
`simple               12288  0`

Просмотрим логи ядра с помощью `dmesg | tail` и увидим

`Hello from the mai 307 team`

Модуль работает.

# Задача №3

Извлечь модуль из ядра. Для этого воспользуемся `sudo rmmod simple` и проверим что модуль извлечён с помощью `dmesg | tail`.

`Goodbye from the mai 307 team!`

При вызове `lsmod | grep simple` модуль больше не отображается.

