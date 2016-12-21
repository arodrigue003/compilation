#ifndef UTILITYFUNCTIONS_HPP
#define UTILITYFUNCTIONS_HPP

#include "expression_symbols.hpp"

// registers and labels counters
int new_var();
int new_label();

void add_identifier(vector<identifier> to_store, struct code_container * cc);

char *double_to_hex_str(double d);

// Identifier gestion
struct expression *load_identifier(char * s, map_boost hash);

#endif // UTILITYFUNCTIONS_HPP
