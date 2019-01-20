CC=cc
CFLAGS=-g -std=c89
LIBS=-pthread
OBJS = lifeMT.o

all: life lifeMT

life: life.o
	$(CC) -o life life.o $(LIBS)

lifeMT: $(OBJS)
	$(CC) -o lifeMT $(OBJS)	$(LIBS)

%.o : %.c
	$(CC) $(CFLAGS) -c $< 

clean:
	rm -rf *.o life lifeMT
