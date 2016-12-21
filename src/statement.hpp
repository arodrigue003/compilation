#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "expression_symbols.hpp"
#include "utilityFunctions.hpp"
#include "expression.hpp"

struct code_container* if_statement(const struct expression &e1, const struct code_container &s1);
struct code_container* if_else_statement(const struct expression &e1, const struct code_container &s1, const struct code_container &s2);

#endif // STATEMENT_HPP
