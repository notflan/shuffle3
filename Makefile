

all: clean shuffle3

clean:
	rm -f build/*

shuffle3:
	gcc  src/*.c -Iinclude/ -g -Wall -pedantic --std=gnu11 -o build/$@ -lm
	./build/$@
