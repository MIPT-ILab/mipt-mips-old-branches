# 
# Building miss_rate_sim
# @author Pavel Poroshin <pavel.a.poroshin@gmail.com>
# Copyright 2013 uArchSim iLab Project
#

TRUNK = ../..

vpath %.h $(TRUNK)/common

# Option for C++ compiler specifying directories 
# to search for headers
INCL = -I ./ -I $(TRUNK)/common/

miss_rate_sim: miss_rate_sim.o cache_tag_array.o
	$(CXX) -o $@ $^
	@echo "---------------------------------"
	@echo "$@ is built successifully"

miss_rate_sim.o: miss_rate_sim.cpp cache_tag_array.h types.h
	$(CXX) -c $< $(INCL)

cache_tag_array.o: cache_tag_array.cpp cache_tag_array.h types.h
	$(CXX) -c $< $(INCL)

clean:
	-rm *.o miss_rate_sim