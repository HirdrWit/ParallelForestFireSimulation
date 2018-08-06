FCC = gcc
LDC = gcc
LD_FLAGS = -std=c11 -fopenmp
FLAGS = -std=c11 -fopenmp
PROG = main.cx
RM = /bin/rm
OBJS = main.o

#all rule
all: $(PROG)

$(PROG): $(OBJS)
    $(LDC) $(LD_FLAGS) $(OBJS) -o $(PROG)

%.o: %.c
    $(FCC) $(FLAGS) -c $<

#clean rule
clean:
    $(RM) -rf *.o $(PROG) *.mod forest_fire.*
