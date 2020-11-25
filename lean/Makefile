SRC_C   = $(wildcard src/*.c)
SRC_CXX = $(wildcard src/*.cpp)
SRC_CXX+= $(wildcard src/rng/*.cpp)

INCLUDE = include

PROJECT=shuffle3

COMMON_FLAGS = -Wall -pedantic $(addprefix -I,$(INCLUDE)) -fno-strict-aliasing

OPT_FLAGS?= -march=native -fgraphite -fopenmp -floop-parallelize-all -ftree-parallelize-loops=4 \
	    -floop-interchange -ftree-loop-distribution -floop-strip-mine -floop-block \
	    -fno-stack-check

CXX_OPT_FLAGS?= $(OPT_FLAGS) -felide-constructors

CFLAGS   += $(COMMON_FLAGS) --std=gnu11
CXXFLAGS += $(COMMON_FLAGS) --std=gnu++20 -fno-exceptions
LDFLAGS  += 

RELEASE_CFLAGS?=   -O3 -flto $(OPT_FLAGS)
RELEASE_CXXFLAGS?= -O3 -flto $(CXX_OPT_FLAGS)
RELEASE_LDFLAGS?=  -O3 -flto

DEBUG_CFLAGS?= -O0 -g -DDEBUG
DEBUG_CXXFLAGS?= $(DEBUG_CFLAGS)
DEBUG_LDFLAGS?=

# Objects

OBJ_C   = $(addprefix obj/c/,$(SRC_C:.c=.o))
OBJ_CXX = $(addprefix obj/cxx/,$(SRC_CXX:.cpp=.o))
OBJ = $(OBJ_C) $(OBJ_CXX)

# Phonies

.PHONY: release
release: | dirs $(PROJECT)-release

.PHONY: debug
debug: | dirs $(PROJECT)-debug

# Targets

dirs:
	@mkdir -p obj/c{,xx}/src
	@mkdir -p obj/cxx/src/rng

obj/c/%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@ $(LDFLAGS)

obj/cxx/%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS) -o $@ $(LDFLAGS)


$(PROJECT)-release: CFLAGS+= $(RELEASE_CFLAGS)
$(PROJECT)-release: CXXFLAGS += $(RELEASE_CXXFLAGS)
$(PROJECT)-release: LDFLAGS += $(RELEASE_LDFLAGS)
$(PROJECT)-release: $(OBJ)
	$(CXX) $^ $(CXXFLAGS) -o $@ $(LDFLAGS)
	strip $@

$(PROJECT)-debug: CFLAGS+= $(DEBUG_CFLAGS)
$(PROJECT)-debug: CXXFLAGS += $(DEBUG_CXXFLAGS)
$(PROJECT)-debug: LDFLAGS += $(DEBUG_LDFLAGS)
$(PROJECT)-debug: $(OBJ)
	$(CXX) $^ $(CXXFLAGS) -o $@ $(LDFLAGS)

clean:
	rm -rf obj
	rm -f $(PROJECT)-{release,debug}
