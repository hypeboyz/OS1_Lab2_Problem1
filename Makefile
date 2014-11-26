obj-m := clock.o
PWD := $(shell pwd)

default: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean: 
	rm -f $(wildcard ./*~)
	rm -f $(wildcard ./*.o)
	rm -f $(wildcard ./*mod*)
	rm -f $(wildcard ./*.ko)
	rm -f $(wildcard ./Module.symvers)
	rm -rf $(wildcard ./.*clock*)
	rm -rf ./.tmp_versions
