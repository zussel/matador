# use "g++" to compile source files.
CXX = g++
# the linker is also "g++". It might be something else with other compilers.
CXXLD = $(CXX)
# Compiler flags go here.
CXXFLAGS = -g -Wall -std=c++0x -I../../include
# Linker flags go here. Currently there aren't any, but if we'll switch to
# code optimization, we might add "-s" here to strip debug info and symbols.
LDFLAGS =
# use this command to erase files.
RM = /bin/rm -f
# source files
SOURCES = \
 test_suite.cpp \
 unit_exception.cpp \
 unit_test.cpp

# sources to compile
OBJECTS = $(SOURCES:.cpp=.o)

# libs to build
LIBRARY = libunit.a

# top-level rule, to compile everything.
all: $(LIBRARY)

# rule for cleaning re-compilable files.
clean:
	$(RM) $(OBJECTS) $(LIBRARY) *~

.PHONY: clean

$(LIBRARY): $(OBJECTS)
	ar rc $(LIBRARY) $(OBJECTS)
	ranlib $(LIBRARY)

# now comes a meta-rule for compiling any "C" source file.
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# now comes a meta-rule for compiling any "C" source file.
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $<
