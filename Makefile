CFLAGS=-O3 -fno-inline -fsanitize=address -Wall -Wextra -pedantic
CXXFLAGS=-O3 -fno-inline -fsanitize=address -Wall -Wextra -pedantic
.PHONY=run run-dlopen clean

run: libmod1.so libmod2.so main
	LD_LIBRARY_PATH=. ./main

run-dlopen: libmod1.so libmod2.so main-dlopen
	./main-dlopen

libmod1.so: mod1.cpp
	$(CXX) $(CXXFLAGS) -shared -fPIC -o libmod1.so mod1.cpp

libmod2.so: mod2.cpp
	$(CXX) $(CXXFLAGS) -shared -fPIC -o libmod2.so mod2.cpp

main-dlopen: main-dlopen.c
	$(CC) $(CFLAGS) -ldl main-dlopen.c -o main-dlopen

main: main.c
	$(CC) $(CFLAGS) main.c -L. -lmod1 -lmod2 -o main

clean:
	rm -f main main-dlopen libmod1.so libmod2.so
