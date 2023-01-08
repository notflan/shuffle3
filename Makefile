SRC_C   = $(wildcard src/*.c)
SRC_CXX = $(wildcard src/*.cpp)
SRC_CXX+= $(wildcard src/rng/*.cpp)

INCLUDE = include

PROJECT=shuffle3

# Currently supported:
#	_FS_SPILL_BUFFER:	Use file backed buffer instead of memory backed one for unshuffling. See `shuffle3.h`.
#				 Setting the value to `DYN` enables the dynamic buffer, setting it to `MAP` enabled memory-mapped buffer. `MAP` is usually the fastest of the 3 modes.
#	DEBUG:		Pretend we're building a debug release even though we're not. Will enable additional output messages and may interfere with some optimisations
FEATURE_FLAGS?=

COMMON_FLAGS+= $(addprefix -D,$(FEATURE_FLAGS)) -Wall -Wstrict-aliasing $(addprefix -I,$(INCLUDE)) -fno-strict-aliasing

ARCH?=native

INTRINSICS=avx sse3

OPT_FLAGS?=-fgraphite -fopenmp -floop-parallelize-all -ftree-parallelize-loops=4 \
	    -floop-interchange -ftree-loop-distribution -floop-strip-mine -floop-block \
	    -fno-stack-check

OPT_FLAGS+=$(addprefix -m,$(INTRINSICS))

ifneq ($(ARCH),)
	OPT_FLAGS+=$(addprefix -march=,$(ARCH))
endif


CXX_OPT_FLAGS?= $(OPT_FLAGS) -felide-constructors

CFLAGS   += $(COMMON_FLAGS) --std=gnu11
CXXFLAGS += $(COMMON_FLAGS) --std=gnu++23 -fno-exceptions
# XXX: We might need exceptions soon, for OOP usage, because we try multiple approaches from most efficient to least.
LDFLAGS  +=  

STRIP=strip

RELEASE_COMMON_FLAGS?= -DNOTRACE
RELEASE_COMMON_FLAGS+= -DRELEASE 

RELEASE_CFLAGS?=   -O3 -flto $(OPT_FLAGS) $(RELEASE_COMMON_FLAGS)
RELEASE_CXXFLAGS?= -O3 -flto $(CXX_OPT_FLAGS) $(RELEASE_COMMON_FLAGS)
RELEASE_LDFLAGS?=  -Wl,-O3 -Wl,-flto

DEBUG_CFLAGS?= -Og -g -DDEBUG
DEBUG_CXXFLAGS?= $(DEBUG_CFLAGS)
DEBUG_LDFLAGS?=

# Objects

OBJ_C   = $(addprefix obj/c/,$(SRC_C:.c=.o))
OBJ_CXX = $(addprefix obj/cxx/,$(SRC_CXX:.cpp=.o))
OBJ = $(OBJ_C) $(OBJ_CXX)

# PGO stuff

PGO_OBJ_C   = $(addprefix obj/pgo/c/,$(SRC_C:.c=.o))
PGO_OBJ_CXX = $(addprefix obj/pgo/cxx/,$(SRC_CXX:.cpp=.o))
PGO_OBJ = $(PGO_OBJ_C) $(PGO_OBJ_CXX)

PGO_ITERATIONS=5
PGO_BLOCKS={1..4}
PGO_SIZE={1024..4096}
PGO_SET_LOC?=/tmp/$(PROJECT)-pgo

PGO_FLAGS  = -fprofile-generate
PGO_LDFLAGS=

# Phonies

.PHONY: release
release: | dirs $(PROJECT)-release

.PHONY: debug
debug: | dirs $(PROJECT)-debug

.PHONY: pgo
pgo: | dirs $(PROJECT)-pgo

.PHONY: test
test: test-all

# Targets

dirs:
	@mkdir -p obj/{pgo/,}c{,xx}/src{,/rng}

obj/c/%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@ $(LDFLAGS)

obj/cxx/%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS) -o $@ $(LDFLAGS)

obj/pgo/c/%.o: %.c
	$(CC) -c $< $(CFLAGS) $(PGO_FLAGS) -o $@ $(LDFLAGS) $(PGO_LDFLAGS)

obj/pgo/cxx/%.o: %.cpp
	$(CXX) -c $< $(CXXFLAGS) $(PGO_FLAGS) -o $@ $(LDFLAGS) $(PGO_LDFLAGS)


$(PROJECT)-release: CFLAGS+= $(RELEASE_CFLAGS)
$(PROJECT)-release: CXXFLAGS += $(RELEASE_CXXFLAGS)
$(PROJECT)-release: LDFLAGS += $(RELEASE_LDFLAGS)
$(PROJECT)-release: $(OBJ)
	$(CXX) $^ $(CXXFLAGS) -o $@ $(LDFLAGS)
	$(STRIP) $@

$(PROJECT)-debug: CFLAGS+= $(DEBUG_CFLAGS)
$(PROJECT)-debug: CXXFLAGS += $(DEBUG_CXXFLAGS)
$(PROJECT)-debug: LDFLAGS += $(DEBUG_LDFLAGS)
$(PROJECT)-debug: $(OBJ)
	$(CXX) $^ $(CXXFLAGS) -o $@ $(LDFLAGS)

pgo-generate: CFLAGS+= $(RELEASE_CFLAGS)
pgo-generate: CXXFLAGS += $(RELEASE_CXXFLAGS)
pgo-generate: LDFLAGS += $(RELEASE_LDFLAGS)
pgo-generate: $(PGO_OBJ)
	$(CXX) $^ $(CXXFLAGS) $(PGO_FLAGS) -o $@ $(LDFLAGS) $(PGO_LDFLAGS)

pgo-reset:
	-find ./obj/pgo -name \*.gcda -exec rm {} +

pgo-profile: | pgo-reset pgo-generate
	mkdir -p $(PGO_SET_LOC)
	for i in {1..$(PGO_ITERATIONS)}; do \
		block=$$(rng --of $(PGO_SIZE)); \
		block_count=$$(rng --of $(PGO_BLOCKS)); \
		dd if=/dev/urandom of=$(PGO_SET_LOC)/small bs=$$block count=$$block_count >> /dev/null 2>&1; \
		printf "Iteration $$i / $(PGO_ITERATIONS) ($$block * $$block_count)\r"; \
		( echo ">> $$i" >&2; \
		  echo ">> $$i"; \
		./pgo-generate -s $(PGO_SET_LOC)/small && \
		./pgo-generate -u $(PGO_SET_LOC)/small && \
		./pgo-generate -h && \
		FCNT=1 ./test.sh ./pgo-generate && \
		FCNT=2 ./test.sh ./pgo-generate && \
		FCNT=3 ./test.sh ./pgo-generate && \
		FCNT=4 ./test.sh ./pgo-generate && \
		: \
		) >>$(PGO_SET_LOC)/stdout.log 2>>$(PGO_SET_LOC)/stderr.log || \exit $$?; \
	done 
	$(shell command -v bat >/dev/null && echo "bat --pager=none" || echo cat)	$(PGO_SET_LOC)/stdout.log; \
	$(shell command -v bat >/dev/null && echo "bat --pager=none" || echo cat)	$(PGO_SET_LOC)/stderr.log >&2
	rm -rf $(PGO_SET_LOC)
	rm pgo-generate

pgo-use: PGO_FLAGS = -fprofile-use
pgo-use: PGO_LDFLAGS = -lgcov -fprofile-use
pgo-use: CFLAGS+= $(RELEASE_CFLAGS)
pgo-use: CXXFLAGS += $(RELEASE_CXXFLAGS)
pgo-use: LDFLAGS += $(RELEASE_LDFLAGS)
pgo-use: $(PGO_OBJ)
	$(CXX) $^ $(CXXFLAGS) $(PGO_FLAGS) -o $@ $(LDFLAGS) $(PGO_LDFLAGS)

$(PROJECT)-pgo: | pgo-profile
	find ./obj/pgo -name \*.o -exec rm {} +
	$(MAKE) pgo-use
	mv pgo-use $@
	$(STRIP) $@

clean-rebuild:
	rm -rf obj

clean: clean-rebuild
	rm -f $(PROJECT)-{release,debug,pgo}

test-all:
	@./test.sh ./$(PROJECT)-*

