#include "../libs/alloc.h"

void register_heap_b(heap_b* b) {
  reg_buffs.buffs[reg_buffs.curr++] = b;
}

void destroy_heap(heap_b* h) {
  free(h->buff);
  h->buff = null;
}

void destroy_heaps() {
  for (int i = 0; i < reg_buffs.curr; i++) {
    destroy_heap(reg_buffs.buffs[i]);
  }
}

heap_b* heap_b_new(heap_b h) {
  static struct {
    size_t curr;
    heap_b heaps[UINT16_MAX];
  } data = {0};
  data.heaps[data.curr].curr = 0;
  data.heaps[data.curr].size = h.size;
  data.heaps[data.curr].grow_size = h.grow_size;
  data.heaps[data.curr].buff = malloc(h.grow_size);
  if (!data.heaps[data.curr].buff)
    err(EXIT_FAILURE, "Heap overflow in heap ctor!");
  return &data.heaps[data.curr++];
}

void heap_b_grow(heap_b* h) {
  h->size += h->grow_size;
  h->buff = realloc((char*)h->buff, h->size);
  if (h == null)
    err(EXIT_FAILURE, "allocation failure Heap Overflow");
}

void* heap_b_append(heap_b* h, void* data) {
  if (data == null)
    err(EXIT_FAILURE, "cannot append a nullptr to heap!");
  if (h->curr >= h->size) {
    heap_b_grow(h);
  }
  void* cell = cast(h->buff, (char*)) + h->curr++ * h->elem_size;
  memcpy(cell, data, h->elem_size);
  return cell;
}

void* hybrd_b_append(hybrd_b* b, void* elem) {
  if (b->curr >= U16_MAX && b->hp == null) {
    b->hp = heap_b_new((heap_b){
      .size = U16_MAX,
      .grow_size = U16_MAX,
      .elem_size = b->elem_size,
    });
    register_heap_b(b->hp);
  }
  if (b->curr >= U16_MAX) {
    return heap_b_append(b->hp, elem);
  }
  void* cell = cast(b->s_buff, (char*)) + b->curr++ * b->elem_size;
  memcpy(cell, elem, b->elem_size);
  return cell;
}
