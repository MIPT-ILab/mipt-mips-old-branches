

# specifying relative path to the TRUNK
TRUNK= ../../

# paths to look for headers
vpath %.h $(TRUNK)/common
INCL= -I ./ -I $(TRUNK)/common/ -I $(TRUNK)/perf_sim/mem

CC=g++
CFLAGS=-c -Wall -MD $(INCL)
LDFLAGS=-MD $(INCL)
CPP_FILES := $(wildcard *.cpp)
D_FILES   := $(wildcard *.d)
OBJ_FILES := $(addprefix ,$(notdir $(CPP_FILES:.cpp=.o)))
EXECUTABLE = miss_rate_sim

all: $(EXECUTABLE)

include $(D_FILES)

$(EXECUTABLE): $(OBJ_FILES)
	$(CC) $(LDFLAGS) $(OBJ_FILES) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(EXECUTABLE) $(OBJ_FILES) $(D_FILES)
