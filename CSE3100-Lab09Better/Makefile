all:
	-gcc counter.c -o counter -pthread
	-gcc dot.c -g -o dot -pthread
	-gcc counter_fixed.c -o counter_fixed -pthread

clean:
	rm -rf counter dot counter_fixed *.o
