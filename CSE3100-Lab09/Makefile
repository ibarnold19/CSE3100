all:
	gcc counter.c -g -o counter -pthread
	gcc dot.c -g -o dot -pthread
	gcc counter_fixed.c -g -o counter_fixed -pthread

clean:
	rm -rf counter dot counter_fixed *.o
