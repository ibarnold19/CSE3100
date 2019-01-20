CLI = rup.o
SRV = rupd.o
CFLAGS = -g
OBJS= data.o

all: rup rupd test

rup: $(CLI) $(OBJS)
	$(CC) $(CFLAGS) $(LFLAGS) -o rup $^

rupd: $(SRV) $(OBJS)
	$(CC) $(CFLAGS) $(LFLAGS) -o rupd $^

test: $(OBJS) libftest.a
	$(CC) $(CFLAGS) $(LFLAGS) -L. -lftest -o ftest $^

libftest.a : ftest.o
	ar -q $@ $^

%.o : %.c
	$(CC) -c $(CFLAGS) -o $@ $^

clean:
	rm -rf *.o rup rupd  *~
