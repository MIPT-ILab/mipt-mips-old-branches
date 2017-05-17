all : cache_tag_array.o miss_rate_sim.o
	g++ cache_tag_array.o miss_rate_sim.o -o output

cache_tag_array.o : cache_tag_array.cpp
	g++ -c cache_tag_array.cpp

miss_rate_sim.o : miss_rate_sim.cpp
	g++ -c miss_rate_sim.cpp

clear : 
	rm *.o output out.txt
