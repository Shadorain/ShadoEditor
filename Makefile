CC=gcc
CFLAGS=-g -Wno-deprecated -Wall -Wextra -pedantic -std=c99 -pie
BDIR=/usr/local/bin/

shado: shado.c
	$(CC) shado.c -o shado $(CFLAGS)
delobj:
	rm -f *.o

clean:
	rm -f *.o shado

install:
	mkdir -p $(BDIR)
	cp -f shado $(BDIR)
	chmod 755 $(BDIR)/shado

uninstall:
	rm -f $(BDIR)/shado

.PHONY: delobj clean install uninstall
