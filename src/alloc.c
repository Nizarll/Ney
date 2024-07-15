#include "../libs/alloc.h"

void register_heap_b(heap_b* b) {
  reg_buffs[curr++] = b;
}

heap_b* heap_b_new(heap_b h) {
  struct {
    size_t curr;
    static heap_b heaps[UINT16_MAX];
  } data = {0};
  data.heaps[data.curr].curr = 0;
  data.heaps[data.curr].size = h.size;
  data.heaps[data.curr].grow_size = h.grow_size;
  data.heaps[data.curr].buff = malloc(h.grow_size);
  if (!data.heaps[data.curr].buff)
    exit(EXIT_FAILURE, "Heap overflow in heap ctor!");
  return &data.heaps[data.curr++];
}

void heap_b_grow(heap_b* h) {
  h->size += h->grow;
  h->buff = realloc((char*)h->buff, h->size);
  if (h == null)
    err(EXIT_FAILURE, "allocation failure Heap Overflow");
}

void heap_b_destroy(heap_b* h) {
  free(h->buff);
  h->buff = null;
}

void heap_b_append(heap_b* h, void* data) {
  if (data == null)
    exit(EXIT_FAILURE, "cannot append a nullptr to heap!");
  if (h->curr >= h->size) {
    heap_b_grow(h);
  }
  memcpy(type_cast(h->buff, (char*)) + h->curr * h->elem_size, data, h->elem_size);
}

void hybrd_b_append(hybrd_b* b, void* elem) {
  if (b->curr >= b->size && b->hp == null) {
    b->hp = heap_new((heap_b){
      .size = h->size;
      .elem_size = b->elem_size;
    });
    register_heap_b(b->hp);
  }
  if (b->curr >= b->size) {
    heap_b_append(h->hp, elem);
    return;
  }
  memcpy(type_cast(b->s_buff, (char*)) + b->curr * b->elem_size, elem, b->elem_size);
}
