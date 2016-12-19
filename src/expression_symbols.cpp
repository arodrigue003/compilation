#include "expression_symbols.h"

#include <stdlib.h>
#include <string.h>

struct expression_symbol* create_expression_symbol_int(int n)
{
  struct expression_symbol *s = (struct expression_symbol *) malloc(sizeof(struct expression_symbol));
  s->t = ENTIER;
  s->v.n = n;
  return s;
}

struct expression_symbol* create_expression_symbol_double(double d)
{
  struct expression_symbol *s = (struct expression_symbol *) malloc(sizeof(struct expression_symbol));
  s->t = _DOUBLE;
  s->v.d = d;
  return s;
}

struct expression* create_expression(enum simple_type t, int var)
{
  struct expression *s = (struct expression *) malloc(sizeof(struct expression));
  s->t = t;
  s->var = var;
  return s;
}
