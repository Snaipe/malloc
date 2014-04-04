#include <unistd.h>
#define SIZE 1024

void *malloc(size_t size) {
    void *c = sbrk(SIZE);
    return c == (void*) -1 ? NULL : c;
}

void free(void *ptr) {}

void *calloc(size_t nmemb, size_t size) {
    void *ptr = malloc(size);
    char *b = ptr;
    for (size_t i = 0; i < SIZE; ++i, ++b) {
        *b = 0;
    }
    return ptr;
}

void *realloc(void *ptr, size_t size) {
    if (!size) return NULL;
    void *newptr = malloc(size);
    if (ptr) {
        char *b1 = ptr, *b2 = newptr;
        for (size_t i = 0; i < SIZE; ++i, ++b1, ++b2) {
            *b2 = *b1;
        }
    }
    return newptr;
}
