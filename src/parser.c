#include "../libs/parser.h"

Program *program_init(Lexem *lexem) { return &(Program){.lexem = lexem}; }
void parse(Program *program) {}
void parse_func(Program *program);
void parse_arrow_func(Program *program);
void parse_expression(Program *program);
void parse_statement(Program *program);
