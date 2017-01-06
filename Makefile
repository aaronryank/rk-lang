#-----Macros---------------------------------

# debug definitions
# VAR_DEBUG, WHILE_DEBUG, LOGIC_DEBUG, PARSE_DEBUG, OP_DEBUG, EQ_DEBUG

CC = gcc
CFLAGS = -g -Wall

#-----Suffix Rules---------------------------

.SUFFIXES: .c

.c.o:
	$(CC) $(CFLAGS) -c $<

.c :
	$(CC) $(CFLAGS) $< -o $@ -lgcc

#-----File Dependencies----------------------
SRC = const.c equation.c fileops.c jump.c logic.c main.c readfile.c rk-func.c rk-main.c strfunc.c variable.c

OBJ = $(addsuffix .o, $(basename $(SRC)))

rk: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

#-----Other stuff----------------------------
depend:
	makedepend $(CFLAGS) -Y $(SRC)

clean:
	rm -f $(OBJ)

reset:
	rm -f $(OBJ) rk
