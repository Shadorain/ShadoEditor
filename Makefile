CC=gcc
SHELL=/bin/sh
CFLAGS=-g -Wno-deprecated -Wall -Wextra -pedantic -std=c99 -pie -pedantic -static-libasan # -fsanitize=address
BDIR=/usr/local/bin/
SRC=./src
OBJ=./obj
LIB=./lib

FILES=shado.c s_abuf.c s_term.c s_rows.c s_ops.c s_bar.c s_io.c s_input.c s_output.c s_modes.c .$(LIB)/rope.c
# FILES=shado.c s_abuf.c s_synhl.c s_term.c s_rows.c s_ops.c s_bar.c s_io.c s_search.c s_input.c s_output.c s_modes.c s_copyreg.c s_stack.c .$(LIB)/rope.c
SOURCES=$(patsubst %,$(SRC)/%,$(FILES))
OBJECTS=$(patsubst %.c,$(OBJ)/%.o,$(FILES))

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -o $@ -c $< $(CFLAGS)

shado: $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS)

delobj:
	rm -f $(OBJ)/*.o

clean:
	rm -f $(OBJ)/*.o ./shado

install:
	mkdir -p $(BDIR)
	cp -f shado $(BDIR)
	chmod 755 $(BDIR)/shado

uninstall:
	rm -f $(BDIR)/shado

valgrind: shado
	valgrind -s --log-file=./.valgrind.log --leak-check=full --show-leak-kinds=all --track-origins=yes ./shado src/shado.c

gdb: shado
	gdb ./shado

.PHONY: delobj clean install uninstall valgrind gdb
