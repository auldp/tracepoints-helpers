obj-m += sched_tp.o

EXTRA_CFLAGS = -I$(src) -D"${DIST_KERN}"

all:
	make -C $(KERNEL_SRC) M=$(PWD) modules

clean:
	make -C $(KERNEL_SRC) M=$(PWD) clean
