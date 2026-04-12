#Makefile
obj-m:=$(FILE).o

#EXTRA_CFLAGS += -DSPNLCK
#EXTRA_CFLAGS += -DMUTEX
EXTRA_CFLAGS += -DSEMPHR

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
