

all: kernel doc

doc:
	make -C doc

kernel:
	make -C src
