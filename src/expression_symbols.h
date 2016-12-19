#ifndef __EXPRESSION_SYMBOLS_H_
#define __EXPRESSION_SYMBOLS_H_

#define SIZE 1013

typedef struct {
  char *name;
  int type;
  char *code;
  char *var;
} symbol_t;

enum simple_type
{
    ENTIER = 0,
    _DOUBLE = 1,
    _INT = 2,
};

union value
{
    int n;
    double d;

};

struct expression_symbol 
{
    enum simple_type t;
    union value v;
};

struct expression
{
    enum simple_type t;
    int var;
    char *indentifier;
    char *code;
};


struct expression_symbol* create_expression_symbol_int(int n); 
struct expression_symbol* create_expression_symbol_double(double d); 
struct expression_symbol* create_expression_symbol_double(double d); 
struct expression* create_expression(enum simple_type t, int var);

#endif // __EXPRESSION_SYMBOLS_H_
