#include "../libs/parser.h"

/* typedef struct Type {
   enum { t_arr, t_fn, t_int, t_float, t_string, ..etc } variant;
   union {
     struct {
       struct Type *elem;
     } case_arr;
     struct {
       struct Type **args;
       struct Type *ret;
       size_t nArgs;
     } case_fn;
   }
 } Type;

 typedef struct Ast {
   enum { lit, binop, unop, expr, ident, string, number, if_st, loop }
   variant; Token tok; Type type; size_t klen; struct Ast kids[];
 } Ast;

typedef struct Lexem lexem; */
