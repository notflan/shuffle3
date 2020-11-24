SRC = $(wildcard src/*.c)
INCLUDE = include/

PROJECT=shuffle3

BUILD=build

OPT_FLAGS?= -march=native -fgraphite -fopenmp -floop-parallelize-all -ftree-parallelize-loops=4 \
	    -floop-interchange -ftree-loop-distribution -floop-strip-mine -floop-block

CFLAGS+= $(addprefix -I,$(INCLUDE)) -Wall -pedantic --std=gnu11
LDFLAGS+= -lm

RELEASE_CFLAGS?= -O3 -flto $(OPT_FLAGS)
RELEASE_LDFLAGS?= -O3 -flto

DEBUG_CFLAGS?= -g -O0
DEBUG_LDFLAGS?= -O0

OBJ = $(addprefix obj/,$(SRC:.c=.o))

.PHONY: release
release: | dirs $(BUILD)/$(PROJECT)-release

.PHONY: debug
debug: | dirs $(BUILD)/$(PROJECT)-debug

dirs:
	@mkdir -p obj/src
	@mkdir -p $(BUILD)

obj/%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@ $(LDFLAGS)

$(BUILD)/$(PROJECT)-release: CFLAGS+= $(RELEASE_CFLAGS)
$(BUILD)/$(PROJECT)-release: LDFLAGS+= $(RELEASE_LDFLAGS)
$(BUILD)/$(PROJECT)-release: $(OBJ)
	$(CC) $^ $(CFLAGS) -o $@ $(LDFLAGS)
	strip $@

$(BUILD)/$(PROJECT)-debug: CFLAGS+= $(DEBUG_CFLAGS)
$(BUILD)/$(PROJECT)-debug: LDFLAGS+= $(DEBUG_LDFLAGS)
$(BUILD)/$(PROJECT)-debug: $(OBJ)
	$(CC) $^ $(CFLAGS) -o $@ $(LDFLAGS)

clean:
	rm -rf obj
	rm -f $(BUILD)/*
