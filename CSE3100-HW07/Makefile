CC=cc
CFLAGS=-g --std=c99 -D_XOPEN_SOURCE=500
LFLAGS= -lpthread -lrt
OFILES= reader.c hist.c

all:	data histogram

data: big.txt

big.txt : big.txt.gz
	gunzip big.txt.gz

big.txt.gz :
	wget ash.engr.uconn.edu/big.txt.gz

histogram: $(OFILES)
	$(CC) $(CFLAGS) $(LFLAGS) $^ -o $@

run:
	histogram big.txt

superclean: clean
	rm -rf big.txt big.txt.gz

clean:
	rm -rf *.o *~ *.fls *.out *.fdb_latexmk *.log *.aux *.synctex.gz histogram 



%.o : %.c
	$(CC) -c $(CFLAGS) $^ -o $@
