#include <unistd.h>
#include <errno.h>

static inline size_t align(size_t size) {
    return size % sizeof(size_t)
        ? size + sizeof(size_t) - size % sizeof(size_t)
        : size;
}

struct chunk {
    struct chunk *next, *prev;
    size_t        size;
    int           free;
    void         *data;
};

typedef struct chunk *Chunk;

static inline size_t chunk_size() {
    static size_t s = 0;
    if (!s) s = align(sizeof(struct chunk));
    return s;
}

static void *malloc_base() {
    static Chunk b = NULL;
    if (!b) {
        b = sbrk(chunk_size());
        if (b == (void*) -1) {
            _exit(127);
        }
        b->next = NULL;
        b->prev = NULL;
        b->size = 0;
        b->free = 0;
        b->data = NULL;
    }
    return b;
}

Chunk malloc_chunk_find(size_t s, Chunk *heap) {
    Chunk c = malloc_base();
    for (; c && (!c->free || c->size < s); *heap = c, c = c->next);
    return c;
}

void *malloc(size_t size) {
    if (!size) return NULL;
    size = align(size);
    Chunk prev = NULL;
    Chunk c = malloc_chunk_find(size, &prev);
    if (!c) {
        Chunk newc = sbrk(size + chunk_size());
        if (newc == (void*) -1) {
            return NULL;
        }
        newc->next = NULL;
        newc->prev = prev;
        newc->size = size;
        newc->data = newc + 1;
        prev->next = newc;
        c = newc;
    }
    c->free = 0;
    return c->data;
}

void free(void *ptr) {
    if (!ptr || ptr < malloc_base() || ptr > sbrk(0)) return;

    Chunk c = (Chunk) ptr - 1;
    if (c->data != ptr) return;
    c->free = 1;
}

void *calloc(size_t nmemb, size_t size) {
    size_t length = nmemb * size;
    void *ptr = malloc(length);
    if (ptr) {
        char *dst = ptr;
        for (size_t i = 0; i < length; *dst = 0, ++dst, ++i);
    }
    return ptr;
}

void *realloc(void *ptr, size_t size) {
    void *newptr = malloc(size);
    if (newptr && ptr) {
        Chunk c = (Chunk) ptr - 1;
        size_t length = c->size > size ? size : c->size;
        char *dst = newptr, *src = ptr;
        for (size_t i = 0; i < length; *dst = *src, ++src, ++dst, ++i);
        free(ptr);
    }
    return newptr;
}
