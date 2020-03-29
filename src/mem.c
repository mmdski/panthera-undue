#include <assert.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

void *
mem_alloc (long nbytes)
{
  return g_malloc (nbytes);
}

void *
mem_calloc (long count, long nbytes)
{
  return g_malloc_n (count, nbytes);
}

void
mem_free (void *ptr)
{
  g_free (ptr);
}
