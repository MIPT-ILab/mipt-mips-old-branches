# 
# Building the cache tag array for MIPS cache
# @author Yury Samarin <yuri.a.samarin@gmail.com>
# Copyright 2013 uArchSim iLab Project
#

vpath %.h ../../common

# option for C++ compiler specifying directories 
# to search for headers
include= -I ./ -I ../../common/

miss_rate_sim: cache_tag_array.o miss_rate_sim.o 
	@# don't forget to link math library using "-lm"
	$(CXX) -o $@ $^ -lm
	@echo "---------------------------------"
	@echo "$@ is built successifully"

cache_tag_array.o: cache_tag_array.cpp cache_tag_array.h types.h
	$(CXX) -c $< $(include)
miss_rate_sim.o: miss_rate_sim.cpp cache_tag_array.h types.h
	$(CXX) -c $< $(include)


clean:
	rm *.o miss_rate_sim
