CC=cc
CFLAGS=-g --std=c99 -D_XOPEN_SOURCE=600
LFLAGS=-lpthread -lrt

all: pism

pism : pism.c
	$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@

clean: 
	rm -rf *.o pism *~
