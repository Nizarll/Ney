#ifndef TYPES

#define I_TYPES($)  \
  $(u, U, 8),        \
  $(u, U, 16),        \
  $(u, U, 32),         \
  $(u, U, 64),          \
  $(i, I, 8),            \
  $(i, I, 16),            \
  $(i, I, 32),             \
  $(i, I, 64)

#define ENUMERATE($) $(SECOND)

enum _primitive_type {
  _primitive_type_none
};

struct _type {

};

typedef struct _type type;
typedef enum _primitive_type _primitive_type;

#endif // !TYPES
