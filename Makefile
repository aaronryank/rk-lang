#-----Macros---------------------------------
INCLUDES = -I.

CC = gcc
CFLAGS = -g $(INCLUDES) -Wall

#-----Suffix Rules---------------------------
# set up C suffixes and relationship between .c and .o files

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
