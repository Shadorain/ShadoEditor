CC=gcc
CFLAGS=-g -Wno-deprecated -Wall -Wextra -pedantic -std=c99 -pie
OBJ=shado.o s_abuf.o s_synhl.o s_term.o s_rows.o s_ops.o
BDIR=/usr/local/bin/

shado: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

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
