CC=mpicc
LDC=mpicc
LD_FLAGS = -std=c11 -lm -fopenmp
FLAGS= -std=c11 -lm -fopenmp
PROGC = main.cx
RM = /bin/rm
OBJSC = main.o

#all rule
all: $(PROGC) $(PROGF)

$(PROGC): $(OBJSC)
	$(LDC) $^ $(LD_FLAGS) -o $@

%C.o: %.c*
	$(CC) $(FLAGS) -c $^ -o $@



#clean rule
clean:
	$(RM) -rf *.o $(PROGF) $(PROGC) *.mod forest_fire.*
