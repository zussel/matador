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
SOURCE = \
 byte_buffer.cpp

# sources to compile
LIB_TOOLS_OBJ = $(SOURCE:.cpp=.o)

OBJS = $(LIB_TOOLS_OBJ)

# libs to build
LIB_TOOLS = libtools.a
LIBS = $(LIB_TOOLS)

# top-level rule, to compile everything.
all: $(LIBS)

# rule for cleaning re-compilable files.
clean:
	$(RM) $(OBJS) $(LIBS) *~

.PHONY: clean

$(LIB_TOOLS): $(LIB_TOOLS_OBJ)
	ar rc $(LIB_TOOLS) $(LIB_TOOLS_OBJ)
	ranlib $(LIB_TOOLS)

# now comes a meta-rule for compiling any "C" source file.
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# now comes a meta-rule for compiling any "C" source file.
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $<
