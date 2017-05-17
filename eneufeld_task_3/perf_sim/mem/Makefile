# 
# Building the miss_rate_sim
# @author Erwin Neufeld <neufeld.erwin@gmail.com>
# Copyright 2012 uArchSim iLab Project
#

# specifying relative path to the TRUNK
TRUNK= ../../

# pathes to loop for headers
vpath %.h $(TRUNK)/common

# option for C++ compiler specifying directories 
# to search for headers
INCL= -I ./ -I $(TRUNK)common/ 

miss_rate_sim: miss_rate_sim.o cache_tag_array.o
	$(CXX) $^ -o $@ $(INCL)
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

miss_rate_sim.o: miss_rate_sim.cpp cache_tag_array.h
	$(CXX) -c $< $(INCL)

cache_tag_array.o: cache_tag_array.cpp cache_tag_array.h
	$(CXX) -c $< $(INCL)

clean:
	@-rm *.o
	@-rm miss_rate_sim
