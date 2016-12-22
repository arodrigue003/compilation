#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "expression_symbols.hpp"
#include "utilityFunctions.hpp"
#include "expression.hpp"

struct code_container* if_then_else(const struct expression &e1, const struct code_container &s1);
struct code_container* if_then_else(const struct expression &e1, const struct code_container &s1, const struct code_container &s2);

struct code_container* for_then(const struct expression &e1, const struct expression &e2, const struct expression &e3, const struct code_container &s1);
struct code_container* for_then(const struct expression &e1, const struct expression &e2, void *,                      const struct code_container &s1);
struct code_container* for_then(const struct expression &e1, void *,                      const struct expression &e3, const struct code_container &s1);
struct code_container* for_then(const struct expression &e1, void *,                      void *,                      const struct code_container &s1);
struct code_container* for_then(void *,                      const struct expression &e2, const struct expression &e3, const struct code_container &s1);
struct code_container* for_then(void *,                      const struct expression &e2, void *,                      const struct code_container &s1);
struct code_container* for_then(void *,                      void *,                      const struct expression &e3, const struct code_container &s1);
struct code_container* for_then(void *,                      void *,                      void *,                      const struct code_container &s1);

struct code_container* while_then(const struct expression &e1, const struct code_container &s1);

struct code_container* do_while(const struct code_container &s1, const struct expression &e1);

#endif // STATEMENT_HPP
