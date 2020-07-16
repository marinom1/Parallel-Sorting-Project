CC = tau_cc.sh
LDC = tau_cc.sh
LD_FLAGS =
FLAGS =
PROG = countingSortParallel.cx
RM = /bin/rm
OBJS = countingSortParallel.o

#all rule
all: $(PROG)

$(PROG): $(OBJS)
	$(LDC) $(LD_FLAGS) $(OBJS) -o $(PROG)

%.o: %.c
	$(CC) $(FLAGS) -c $<

#clean
clean:
	$(RM) -rf *.o $(PROG) *.mod
