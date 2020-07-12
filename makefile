CC = gcc
LDC = gcc
LD_FLAGS =
FLAGS =
PROG = countingSortSerial.cx
RM = /bin/rm
OBJS = countingSortSerial.o

#all rule
all: $(PROG)

$(PROG): $(OBJS)
	$(LDC) $(LD_FLAGS) $(OBJS) -o $(PROG)

%.o: %.c
	$(CC) $(FLAGS) -c $<

#clean
clean:
	$(RM) -rf *.o $(PROG) *.mod