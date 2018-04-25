#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

typedef void (*fun_t)(void);

int main(void) {
    void *handle1 = dlopen("./libmod1.so", RTLD_LAZY | RTLD_GLOBAL);
    if (!handle1) {
        fprintf(stderr, "dlopen mod1 failed: %s\n", dlerror());
        exit(1);
    }

    void *handle2 = dlopen("./libmod2.so", RTLD_LAZY | RTLD_GLOBAL);
    if (!handle2) {
        fprintf(stderr, "dlopen mod2 failed: %s\n", dlerror());
        exit(1);
    }

    fun_t fun;
    (void) dlerror(); // clear error
    *(void **) &fun = dlsym(handle2, "Open");
    const char *err = dlerror();
    if (err) {
        fprintf(stderr, "failed to load Open symbol: %s\n", err);
        exit(1);
    }

    if (!fun) {
        fprintf(stderr, "Symbol is NULL\n");
        exit(1);
    }

    (*fun)();
    return 0;
}
