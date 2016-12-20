#ifndef UTILITYFUNCTIONS_HPP
#define UTILITYFUNCTIONS_HPP

#include "expression_symbols.hpp"

#include <boost/unordered_map.hpp>
typedef boost::unordered_map<std::string, struct identifier> map_boost;

// registers and labels counters
int new_var();
int new_label();


void add_identifier(vector<identifier> to_store, struct code_container * cc);

// Identifier gestion
struct expression *load_identifier(char * s, map_boost hash);

// Code generation for basics operations between expressions
struct expression *expression_addition(struct expression * e1, struct expression * e2);
struct expression *expression_soustraction(struct expression * e1, struct expression * e2);
struct expression *expression_multiplication(struct expression * e1, struct expression * e2);
struct expression *expression_quotient(struct expression * e1, struct expression * e2);
struct expression *expression_reste(struct expression * e1, struct expression * e2);

// Code generation for assignments
struct expression *assign_equal(struct expression * e1, char * s, map_boost hash);

#endif // UTILITYFUNCTIONS_HPP
