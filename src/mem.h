extern void *
mem_alloc(long nbytes, const char *file, int line);

extern void *
mem_calloc(long count, long nbytes, const char *file, int line);

extern void
mem_free(void *ptr, const char *file, int line);

#define ALLOC(nbytes) mem_alloc((nbytes), __FILE__, __LINE__)
#define NEW(p) ((p) = ALLOC((long) sizeof *(p)))
#define FREE(ptr) ((void) (mem_free((ptr), __FILE__, __LINE__), (ptr) = 0))
