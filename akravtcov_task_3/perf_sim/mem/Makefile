# 
# Building the miss rate study
# author Alexander Kravtcov
#

TRUNK= ../../

# paths to look for headers
vpath %.h $(TRUNK)/common

# option for C++ compiler specifying directories 
# to search for headers
INCL= -I ./ -I $(TRUNK)/common/ 

CXX_FLAGS := -Wall
ifeq ($(debug), 1)
	CXX_FLAGS := $(CXX_FLAGS) -g -O0 -DDEBUG
endif

miss_rate_sim: miss_rate_sim.o cache_tag_array.o 
	$(CXX) -o $@ $^ $(CXX_FLAGS) 
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

miss_rate_sim.o: miss_rate_sim.cpp cache_tag_array.h types.h
	$(CXX) -c $< $(INCL) $(CXX_FLAGS)

cache_tag_array.o: cache_tag_array.cpp cache_tag_array.h types.h
	$(CXX) -c $< $(INCL) $(CXX_FLAGS)


clean:
	@-rm -f *.o
	@-rm -f miss_rate_sim
