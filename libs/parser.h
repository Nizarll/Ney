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

} Base_Type;

typedef enum {
  PROGRAM,
  STATEMENT,
  DEC,
  FUNCTION_DEC,
  ARR_FUNCTION_DEC,
  FUNCTION_CALL,
  ASSIGNMENT,
  OPERATION, // i += , i != , i /= , i %= , i &= , i |= etc i++, i--
} Node_Type;

typedef enum {
  IF,
  LOOP,
  CALL,
  RETURN,
} Statement_Type;

typedef struct Node {
  Node_Type type;

  union {

    struct {
      Token_Type op;
      Node *left;
      Node *right;
    } expression;

    struct {
      Statement_Type type;
      Node *args;
      Node *condition;
      Node *body;
    } statement;

    struct {
      Base_Type type;
      char *name;
      Node *value;
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
  };

} Node;

typedef struct {
  Node body;
} Program;

#endif // PARSER_H

