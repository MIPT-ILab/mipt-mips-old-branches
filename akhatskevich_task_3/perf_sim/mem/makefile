TRUNC= ../..
CFLAGS= -Wall -I $(TRUNC)/common/ -O2
CC= g++

all: miss_rate_sim

miss_rate_sim: miss_rate_sim.cpp cache_tag_array.h cache_tag_array.o
	$(CC) $(CFLAGS) miss_rate_sim.cpp cache_tag_array.o -o miss_rate_sim

cache_tag_array.o: cache_tag_array.cpp cache_tag_array.h
	$(CC) $(CFLAGS) cache_tag_array.cpp -c 

clean:
	rm *.o miss_rate_sim
run: miss_rate_sim mem_trace.txt
	./miss_rate_sim mem_trace.txt res.csv

