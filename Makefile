CC=gcc
CFLAGS=-g -Wno-deprecated -Wall -Wextra -pedantic -std=c99 -pie -pedantic -static-libasan # -fsanitize=address
OBJ=shado.o s_abuf.o s_synhl.o s_term.o s_rows.o s_ops.o s_bar.o s_io.o s_search.o s_input.o s_output.o s_modes.o s_copyreg.o s_stack.o
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

valgrind: shado
	valgrind --log-file=./.valgrind.log --leak-check=full --show-leak-kinds=all --track-origins=yes ./shado shado.c

gdb: shado
	gdb ./shado

.PHONY: delobj clean install uninstall valgrind gdb
