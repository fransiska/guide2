#Release_flags g
all:
	$(MAKE) -f Makeguide4

clean:
	make -f Makeguide4 clean

cleandep:
	make -f Makeguide4 cleandep

createdep:
	make -f Makeguide4 createdep

list:
	make -f Makeguide4 list
