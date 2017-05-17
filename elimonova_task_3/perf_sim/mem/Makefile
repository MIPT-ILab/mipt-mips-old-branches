# 
# Building the miss_rate_sim 
# Copyright 2012 uArchSim iLab Project
#

vpath %.h ../../common
vpath %.h .
vpath %.cpp .

# option for C++ compiler specifying directories 
# to search for headers
include=-I ./ -I ../../common/

miss_rate_sim: miss_rate_sim.o cache_tag_array.o cache_tag_array.h
	$(CXX) -o $@ $^ $(include) 
	@echo "---------------------------------"
	@echo "$@ is built successifully"

miss_rate_sim.o: miss_rate_sim.cpp cache_tag_array.h
	$(CXX) -c $< $(include)

cache_tag_array.o: cache_tag_array.cpp cache_tag_array.h types.h
	$(CXX) -c $< $(include)
clean:
	-rm *.o miss_rate_sim
