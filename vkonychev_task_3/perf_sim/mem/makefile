CPPFLAGS = -Wall
CPPLIB = -lm
CPP = g++


main : miss_rate_sim.o cache_tag_array.o
	$(CPP) $(CPPFLAGS) $^ -o $@ $(CPPLIB)

miss_rate_sim.o: miss_rate_sim.cpp cache_tag_array.h
	$(CPP) -c $(CPPFLAGS) $< $(CLIB)

cache_tag_array.o: cache_tag_array.cpp cache_tag_array.h types.h
	$(CPP) -c $(CPPFLAGS) $< $(CLIB)  

clean:
	rm -fr *.o 	
