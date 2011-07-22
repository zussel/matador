# archive command
TAR = tar
TARFLAGS = cvjf
TAR_EXTRA = --exclude '.git'
TAREXT = .tar.bz2
PROJECT_DIR = oos

all: libs tests

#applications:
#	make -C apps

libs:
	make -C src

tests:
	make -C test

clean:
	make -C test clean
	make -C src clean

archive: clean
	$(TAR) $(TARFLAGS) ../$(PROJECT_DIR)$(TAREXT) $(TAR_EXTRA) ../$(PROJECT_DIR)

.PHONY: clean
