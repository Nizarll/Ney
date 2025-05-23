#ifndef LEXER
#define LEXER

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

#define ney_alloc(x)        malloc(x)
#define ney_free(x)         free(x)
#define ney_realloc(ptr, x) malloc(x)

#define TokenKinds($) \
  $(END_OF_FILE),      \
  $(PLUS),              \
  $(PIPE),               \
  $(ISEQ),                \
  $(NOTEQ),                \
  $(EQUAL),                 \
  $(MINUS),                  \
  $(DIV),                     \
  $(DOT),                      \
  $(NOT),                       \
  $(ARROW),                      \
  $(LESSEQ),                      \
  $(SYMBOL),                       \
  $(IDENTIFIER),                    \
  $(STRLIT),                         \
  $(CHARLIT),                         \
  $(NUMLIT),                           \
  $(SEMICOL),                           \
  $(GREATER),                           \
  $(OPENBRACK),                         \
  $(MUL),                                \
  $(COMMA),                               \
  $(LESS),                                 \
  $(COLON),                                 \
  $(CLOSECURLY),                             \
  $(CHAR),                                    \
  $(STR),                                      \
  $(SHIFTLEFT),                                 \
  $(SHIFTRIGHT),                                 \
  $(GREATEREQ),                                   \
  $(OPENPAREN),                                    \
  $(CLOSEPAREN),                                    \
  $(CLOSEBRACK),                                     \
  $(OPENCURLY)

#define __stringify(str) #str
#define stringify(str) __stringify(str) // pass the macro down to another macro to cause expansion


#define ENUMERATE($) $
#define STRING($) string(stringify($))

enum _token_kind {
  TokenKinds(ENUMERATE),
  TokenKindLen,
};

static const string TokenKindStrings[] = { TokenKinds(STRING) };

#undef TokenKinds
#undef ENUMIZE
#undef STRING

struct _token {
  const char* view;
  struct _span span;
  enum _token_kind kind;
};

struct _lexer {
  uint i;
  uint col; //TODO:
  uint row;
  location loc;
};

struct _token_list {
  struct _token* tokens;
  usz occupied;
  usz total;
};

#define foreach(token, list) for (struct _token* token = list.tokens; token != list.tokens + list.occupied; token++)

typedef struct _token_list token_list;
typedef struct _token token;
typedef struct _lexer lexer;
typedef enum _token_kind token_kind;

token_list lexer_lex(struct _lexer* lexer, any_string view);

#endif // ! LEXER