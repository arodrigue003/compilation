#include "expression_symbols.hpp"

#include <stdlib.h>
#include <string.h>

expression::expression(simple_type t, int var) : t(t), var(var) {}

simple_type expression::getT() const {
    return t;
}

int expression::getVar() const {
    return var;
}

expression::~expression() {
}
