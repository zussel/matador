# use "g++" to compile source files.
CXX = g++
# the linker is also "g++". It might be something else with other compilers.
CXXLD = $(CXX)
# Compiler flags go here.
CXXFLAGS = -g -std=c++0x -Wall -I../../include
# Linker flags go here. Currently there aren't any, but if we'll switch to
# code optimization, we might add "-s" here to strip debug info and symbols.
LDFLAGS = -L../../src/tools -L../../src/unit
# use this command to erase files.
RM = /bin/rm -f
# source
SOURCES = \
 test_unit.cpp

# sources to compile
OBJECTS = ${SOURCES:.cpp=.o}

# libraries to link
LIBS = -ltools -lunit

# program executable file name.
PROGRAM = test_unit

# top-level rule, to compile everything.
all: $(PROGRAM)

# rule for cleaning re-compilable files.
clean:
	$(RM) $(PROGRAM) $(OBJECTS) *~

.PHONY: clean

# rule to link the program
$(PROGRAM): $(OBJECTS)
	$(CXXLD) $(LDFLAGS) $(OBJECTS) -o $(PROGRAM) $(LIBS)

# now comes a meta-rule for compiling any "C" source file.
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# now comes a meta-rule for compiling any "C" source file.
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $<
