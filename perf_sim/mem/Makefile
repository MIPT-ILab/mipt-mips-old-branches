
INCL=-I ../../common/ -I ./

.PHONY: all
all: miss_rate_sim

miss_rate_sim: miss_rate_sim.o cache_tag_array.o
	g++ $(INCL) $^ -o $@ -l elf
	@echo '=== disasm built succesful'

miss_rate_sim.o: miss_rate_sim.cpp cache_tag_array.h
	g++ $(INCL) -c $<

cache_tag_array.o: cache_tag_array.cpp cache_tag_array.h
	g++ $(INCL) -c $<

.PHONY: clean
clean: 
	rm -rf *.o miss_rate_sim

