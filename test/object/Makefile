# use "g++" to compile source files.
CXX = g++
# the linker is also "g++". It might be something else with other compilers.
CXXLD = $(CXX)
# Compiler flags go here.
CXXFLAGS = -g -std=c++0x -Wall -I../../include
# Linker flags go here. Currently there aren't any, but if we'll switch to
# code optimization, we might add "-s" here to strip debug info and symbols.
LDFLAGS = -L../../src/object
# use this command to erase files.
RM = /bin/rm -f
# source
SOURCE = \
 test_object.cpp \
 Album.cpp \
 Artist.cpp \
 Track.cpp

# sources to compile
test_object_PROG_OBJ = ${SOURCE:.cpp=.o}

PROG_OBJ = \
 $(test_object_PROG_OBJ)

LIBS = -lobject

# program executable file name.
test_object_PROG = test_object

PROGS = \
 $(test_object_PROG)

# top-level rule, to compile everything.
all: $(PROGS)

# rule for cleaning re-compilable files.
clean:
	$(RM) $(PROGS) $(PROG_OBJ) *~

.PHONY: clean

# rule to link the program
$(test_object_PROG): $(test_object_PROG_OBJ)
	$(CXXLD) $(LDFLAGS) $(test_object_PROG_OBJ) -o $(test_object_PROG) $(LIBS)

# now comes a meta-rule for compiling any "C" source file.
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# now comes a meta-rule for compiling any "C" source file.
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $<
