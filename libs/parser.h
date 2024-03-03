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
      struct Node *left;
      struct Node *right;
    } expression;

    struct {
      Statement_Type type;
      struct Node *args;
      struct Node *condition;
      struct Node *body;
    } statement;

    struct {
      Base_Type type;
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
  Lexem *lexem;
  Node curr_node;
} Program;

Program *program_init(Lexem *lexem);
void parse_func(Program *program);
void parse_arrow_func(Program *program);
void parse_expression(Program *program);
void parse_statement(Program *program);

#endif // PARSER_H
