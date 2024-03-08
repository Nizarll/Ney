#ifndef PARSER_H
#define PARSER_H

#include "./lexer.h"

typedef enum {
  INT,
  UINT,
  FLOAT,
  FUNCTION,
  STRING,
  CHAR,
} Type;

typedef enum {
  PROGRAM,
  STATEMENT,
  DEC,
  FUNCTION_DEC,
  ARR_FUNCTION_DEC,
  FUNCTION_CALL,
  ASSIGNMENT,
  OPERATION, // i += , i != , i /= , i %= , i &= , i |= etc i++, i--
} Node_Kind;

typedef enum {
  IF,
  LOOP,
  CALL,
  RETURN,
} Statement_Kind;

typedef struct Node {
  Node_Kind type;

  union {

    struct {
      Token_Kind op;
      struct Node *left;
      struct Node *right;
    } expression;

    struct {
      Statement_Kind type;
      struct Node *args;
      struct Node *condition;
      struct Node *body;
    } statement;

    struct {
      Type type;
      char *name;
      struct Node *value;
    } declaration;

    //    struct {
    //
    //    } func_declaration;
    //
    //    struct {
    //
    //    } arr_func_declaration;
    //
    //    struct {
    //
    //    } operation;
    //
  };

} Node;

typedef struct {
  Node *body;
  Node curr_node;
} Program;

void parse_func(Program *program);
void parse_arrow_func(Program *program);
void parse_expression(Program *program);
void parse_statement(Program *program);

#endif // PARSER_H
