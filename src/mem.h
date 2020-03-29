extern void *mem_alloc (long nbytes);

extern void *mem_calloc (long count, long nbytes);

extern void mem_free (void *ptr);

#define ALLOC(nbytes) mem_alloc ((nbytes))
#define NEW(p) ((p) = ALLOC ((long) sizeof *(p)))
#define FREE(ptr) ((void) (mem_free ((ptr)), (ptr) = 0))
