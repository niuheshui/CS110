# CS110 search Makefile Hooks

PROGS = search imdbtest generator
CXX = /usr/bin/g++

# 	-Wconversion 
# 	-Wsign-conversion 
# 	-Wold-style-cast 
CXX_WARNINGS = -Wall \
	-Wextra \
	-Wpedantic \
	-Wnull-dereference \
	-Woverloaded-virtual \
	-Wnon-virtual-dtor \
	-Wshadow \
	-Werror \
	-Wno-error=unused-parameter \
	-Wno-error=unused-variable 

CXX_DEPS = -MMD -MF $(@:.o=.d)
CXX_DEFINES =
CXX_INCLUDES = -I../extra/include

CXXFLAGS = -g $(CXX_WARNINGS) -O0 -std=c++20 $(CXX_DEPS) $(CXX_DEFINES) $(CXX_INCLUDES)
LDFLAGS = 

LIB_SRC = imdb.cc path.cc imdb-utils.cc
LIB_OBJ = $(patsubst %.cc,%.o,$(patsubst %.S,%.o,$(LIB_SRC)))
LIB_DEP = $(patsubst %.o,%.d,$(LIB_OBJ))
LIB = libsearch.a

PROGS_SRC = $(patsubst %,%.cc,$(PROGS))
PROGS_OBJ = $(patsubst %.cc,%.o,$(patsubst %.S,%.o,$(PROGS_SRC)))
PROGS_DEP = $(patsubst %.o,%.d,$(PROGS_OBJ))

all:: $(PROGS)

$(PROGS) $(EXTRA_PROGS): %:%.o $(LIB)
	$(CXX) $^ $(LDFLAGS) -o $@

$(LIB): $(LIB_OBJ)
	rm -f $@
	ar r $@ $^
	ranlib $@

compile_commands.json: 
	compiledb make -Bn

clean::
	rm -f $(PROGS) $(PROGS_OBJ) $(PROGS_DEP)
	rm -f $(LIB) $(LIB_OBJ) $(LIB_DEP)

spartan:: clean
	\rm -fr *~

.PHONY: all clean spartan compile_commands.json

-include $(PROGS_DEP) $(LIB_DEP) $(LIB_DEP)
