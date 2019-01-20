CC=cc
CFLAGS=-g --std=c99 -D_XOPEN_SOURCE=600
LIBS = -lpthread
OBJS = life.o
OBJSMT = lifeMT.o
OBJSMT1 = lifeMT1.o
OBJSMT2 = lifeMT2.o

all: life lifeMT lifeMT1 lifeMT2

life: $(OBJS)
	$(CC) -o life $^  $(LIBS)

lifeMT: $(OBJSMT)
	$(CC) -o lifeMT  $^ $(LIBS)

lifeMT1: $(OBJSMT1)
	$(CC) -o lifeMT1  $^ $(LIBS)

lifeMT2: $(OBJSMT2)
	$(CC) -o lifeMT2  $^ $(LIBS)

%.o : %.c
	$(CC) $(CFLAGS) -c $< 

clean:
	rm -rf *.o life lifeMT lifeMT2 lifeMT1

