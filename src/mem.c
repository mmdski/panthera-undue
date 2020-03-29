#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void *
mem_alloc(long nbytes, const char *file, int line)
{

    assert(nbytes > 0);
    void *ptr;

    ptr = malloc(nbytes);

    if (ptr == NULL) {
        if (file == NULL)
            fprintf(stderr, "Memory allocation failure");
        else {
            fprintf(stderr, "Memory allocation failure %s:%d", file, line);
        }
        abort();
    }

    return ptr;
}

void *
mem_calloc(long count, long nbytes, const char *file, int line)
{
    assert(count > 0);
    assert(nbytes > 0);

    void *ptr;

    ptr = calloc(count, nbytes);

    if (ptr == NULL) {
        if (file == NULL)
            fprintf(stderr, "Memory allocation failure");
        else {
            fprintf(stderr, "Memory allocation failure %s:%d", file, line);
        }
        abort();
    }

    return ptr;
}

void
mem_free(void *ptr, const char *file, int line)
{
    if (ptr)
        free(ptr);
}
