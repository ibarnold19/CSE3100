CLI = client.o
SRV = server.o
OBJS = 
CFLAGS = -g --std=c99

all: rshd rsh 

rsh: $(CLI) $(OBJS)
	$(CC) $(CFLAGS) $(LFLAGS) -o rsh $^

rshd: $(SRV) $(OBJS)
	$(CC) $(CFLAGS) $(LFLAGS) -o rshd $^

clean:
	rm -rf *.o rsh rshd *~
