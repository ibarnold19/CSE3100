CLI = rup.c
SRV = rupd.c
CFLAGS = -g
OBJS= data.c

all: rup rupd demo

rup: $(CLI) $(OBJS)
	$(CC) $(CFLAGS) $(LFLAGS) -o rup $^

rupd: $(SRV) $(OBJS)
	$(CC) $(CFLAGS) $(LFLAGS) -o rupd $^

demo: $(OBJS) libftest.a
	$(CC) $(CFLAGS) $(LFLAGS) -L. -lftest -o $@  $^

%.o : %.c
	$(CC) -c $(CFLAGS) -o $@ $^

clean:
	rm -rf *.o rup rupd  demo  *~
