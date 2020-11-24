



test:
	gcc -c src/*.c -Iinclude -Wall -pedantic --std=gnu11
	g++ -c src/*.cpp -Iinclude -Wall -pedantic --std=gnu++20
	g++ -o $@ *.o
	./$@
clean:
	rm -f test
	rm -f *.o
