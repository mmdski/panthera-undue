#include "list.h"
#include "mem.h"
#include <assert.h>
#include <stdlib.h>

typedef struct Node Node;

struct Node {
    void *data;
    Node *next;
};

static Node *
node_new(void *data)
{
    Node *node;
    NEW(node);
    node->data = data;
    node->next = NULL;
    return node;
}

static void
node_free(Node *node)
{
    assert(node);
    FREE(node);
}

struct List {
    int   length;
    Node *head;
    Node *tail;
};

List
list_new(void)
{
    List list;
    NEW(list);
    list->length = 0;
    list->head   = NULL;
    list->tail   = NULL;

    return list;
}

void
list_free(List list)
{
    assert(list);
    Node *node = list->head;
    Node *prev;
    while (node) {
        prev = node;
        node = node->next;
        node_free(prev);
    }
    FREE(list);
}

void
list_append(List list, void *data)
{
    assert(list);
    Node *node = node_new(data);
    if (list->length == 0) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail       = node;
    }
    list->length++;
}

int
list_length(List list)
{
    assert(list);
    return list->length;
}

void *
list_to_array(List list)
{
    assert(list);

    if (list->length == 0)
        return NULL;

    void **array =
        mem_calloc(list->length, sizeof(void *), __FILE__, __LINE__);
    int   i    = 0;
    Node *node = list->head;
    while (node) {
        array[i++] = node->data;
        node       = node->next;
    }

    return array;
}
