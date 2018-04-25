This project illustrates [ODR] undefined behavior.

[ODR]: http://en.cppreference.com/w/cpp/language/definition#One_Definition_Rule

## Objective

[Chouquette] demonstrated that we can observe undefined behavior by violating
C++ ODR.

This project adds the following constaints:
 - two independant C++ modules (as shared libraries) define two classes with the
   same name;
 - the modules are loaded from a C program (either linked or with `dlopen()`);
 - everything is compiled with optimizations enabled (`-03`).

[chouquette]: https://gist.github.com/chouquette/d6414f5a1cd2bb0db93b43f88666d2ad

## Run

    make run          # the linked version
    make run-dlopen   # the dlopen version

## Results

```
$ make run
g++ -O3 -fno-inline -fsanitize=address -Wall -Wextra -pedantic -shared -fPIC -o libmod1.so mod1.cpp
g++ -O3 -fno-inline -fsanitize=address -Wall -Wextra -pedantic -shared -fPIC -o libmod2.so mod2.cpp
cc -O3 -fno-inline -fsanitize=address -Wall -Wextra -pedantic main.c -L. -lmod1 -lmod2 -o main
LD_LIBRARY_PATH=. ./main
=================================================================
==23282==ERROR: AddressSanitizer: new-delete-type-mismatch on 0x603000000010 in thread T0:
  object passed to delete has wrong type:
  size of the allocated type:   32 bytes;
  size of the deallocated type: 40 bytes.
    #0 0x7fba6d6006c8 in operator delete(void*, unsigned long) (/usr/lib/x86_64-linux-gnu/libasan.so.4+0xdc6c8)
    #1 0x55e3abd3b74d in main (/home/rom/projects/experiments/odr/main+0x74d)
    #2 0x7fba6cd85a86 in __libc_start_main (/lib/x86_64-linux-gnu/libc.so.6+0x21a86)
    #3 0x55e3abd3b7a9 in _start (/home/rom/projects/experiments/odr/main+0x7a9)

0x603000000010 is located 0 bytes inside of 32-byte region [0x603000000010,0x603000000030)
allocated by thread T0 here:
    #0 0x7fba6d5ff340 in operator new(unsigned long) (/usr/lib/x86_64-linux-gnu/libasan.so.4+0xdb340)
    #1 0x7fba6d11f31a in Open (libmod2.so+0x131a)

SUMMARY: AddressSanitizer: new-delete-type-mismatch (/usr/lib/x86_64-linux-gnu/libasan.so.4+0xdc6c8) in operator delete(void*, unsigned long)
==23282==HINT: if you don't care about these errors you may set ASAN_OPTIONS=new_delete_type_mismatch=0
==23282==ABORTING
make: *** [Makefile:6: run] Error 1
```

```
$ make run-dlopen
g++ -O3 -fno-inline -fsanitize=address -Wall -Wextra -pedantic -shared -fPIC -o libmod1.so mod1.cpp
g++ -O3 -fno-inline -fsanitize=address -Wall -Wextra -pedantic -shared -fPIC -o libmod2.so mod2.cpp
cc -O3 -fno-inline -fsanitize=address -Wall -Wextra -pedantic -ldl main-dlopen.c -o main-dlopen
./main-dlopen
=================================================================
==23333==ERROR: AddressSanitizer: new-delete-type-mismatch on 0x603000000010 in thread T0:
  object passed to delete has wrong type:
  size of the allocated type:   32 bytes;
  size of the deallocated type: 40 bytes.
    #0 0x7f40058c96c8 in operator delete(void*, unsigned long) (/usr/lib/x86_64-linux-gnu/libasan.so.4+0xdc6c8)
    #1 0x556b6c08dcab in main (/home/rom/projects/experiments/odr/main-dlopen+0xcab)
    #2 0x7f4005250a86 in __libc_start_main (/lib/x86_64-linux-gnu/libc.so.6+0x21a86)
    #3 0x556b6c08ddc9 in _start (/home/rom/projects/experiments/odr/main-dlopen+0xdc9)

0x603000000010 is located 0 bytes inside of 32-byte region [0x603000000010,0x603000000030)
allocated by thread T0 here:
    #0 0x7f40058c8340 in operator new(unsigned long) (/usr/lib/x86_64-linux-gnu/libasan.so.4+0xdb340)
    #1 0x7f40015fb31a in Open (libmod2.so+0x131a)

SUMMARY: AddressSanitizer: new-delete-type-mismatch (/usr/lib/x86_64-linux-gnu/libasan.so.4+0xdc6c8) in operator delete(void*, unsigned long)
==23333==HINT: if you don't care about these errors you may set ASAN_OPTIONS=new_delete_type_mismatch=0
==23333==ABORTING
make: *** [Makefile:9: run-dlopen] Error 1
```

## Limitations

For the _dlopen_ version, the crash only happens if `dlopen()` is called with
`RTLD_GLOBAL`.

The crash does not happen without `-fno-inline`. But we could make the program
arbitrarily complex so that the compiler does not inline the functions.

To be able to call the methods from C, the types defined in `.cpp` files must be
public (by default, `struct` is `public` while `class` is private in C++).
Instead, we could make them private, and use a friend function that would access
it.
