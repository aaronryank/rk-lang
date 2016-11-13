OBJ := main.o readfile.o strfunc.o rk-main.o rk-func.o const.o variable.o logic.o equation.o jump.o fileops.o
CC := gcc
CFLAGS := -g -Wall # -DVAR_DEBUG -DWHILE_DEBUG -DLOGIC_DEBUG -DPARSE_DEBUG -DOP_DEBUG -DEQ_DEBUG

EXECUTABLE := rk

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXECUTABLE)

clean:
	rm $(OBJ)

reset:
	rm $(OBJ) $(EXECUTABLE)
