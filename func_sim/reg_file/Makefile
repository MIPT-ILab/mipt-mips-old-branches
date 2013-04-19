# 
# Building the register file 
# author Alexander Kravtcov
#

vpath %.h ../../common

include=-I ./ -I ../../common

ifeq ($(debug), 1)
	CXX_FLAGS := -g -O0 -Wall -DDEBUG
endif

test: test.o reg_file.o 
	$(CXX) -o $@ $^  $(CXX_FLAGS)
	@echo "---------------------------------"
	@echo "$@ is built successifully"

test.o: test.cpp reg_file.h types.h
	$(CXX) -c $< $(include) $(CXX_FLAGS)

reg_file.o: reg_file.cpp reg_file.h types.h
	$(CXX) -c $< $(include) $(CXX_FLAGS)

clean:
	rm -f *.o test
