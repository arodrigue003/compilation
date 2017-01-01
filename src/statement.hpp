#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "utilityFunctions.hpp"
#include "expression.hpp"

struct code_container* if_then_else(const struct expression& e1,
                                    const struct code_container& s1);
struct code_container* if_then_else(const struct expression& e1,
                                    const struct code_container& s1, const struct code_container& s2);

struct code_container* for_then(const struct expression& e1, const struct expression& e2, const struct expression& e3, const struct code_container& s1);
struct code_container* for_then(const struct expression& e1, const struct expression& e2, void*,                       const struct code_container& s1);
struct code_container* for_then(const struct expression& e1, void*,                       const struct expression& e3, const struct code_container& s1);
struct code_container* for_then(const struct expression& e1, void*,                       void*,                       const struct code_container& s1);
struct code_container* for_then(void*,                       const struct expression& e2, const struct expression& e3, const struct code_container& s1);
struct code_container* for_then(void*,                       const struct expression& e2, void*,                       const struct code_container& s1);
struct code_container* for_then(void*,                       void*,                       const struct expression& e3, const struct code_container& s1);
struct code_container* for_then(void*,                       void*,                       void*,                       const struct code_container& s1);

struct code_container* while_then(const struct expression& e1, const struct code_container& s1);

struct code_container* do_while(const struct code_container& s1, const struct expression& e1);


//Function to add p5 functions to the hash table in order to recognize them
void setup_p5(map_boost &hash);

// used for function declaration
struct code_container* declare_q5_used_functions(map_boost &hash_table);
struct code_container* declare_funct(enum simple_type t, string name, map_boost &hash_table);
struct code_container* declare_funct(enum simple_type t, string name, struct type_list tl, map_boost &hash_table);

// declaration
struct code_container* local_declaration(enum simple_type t, struct declaration_list &decla, map_boost &hash_table);
struct code_container* global_declaration(enum simple_type t, struct declaration_list &decla, map_boost &hash_table);

// return
struct code_container *return_statement(struct expression& e1);

// Function definition
struct code_container *define_funct(enum simple_type t, string name, struct declaration_list *decla, struct code_container *code, map_boost &hash_table);
struct code_container *define_funct(enum simple_type t, string name, struct code_container *code, map_boost &hash_table);

#endif // STATEMENT_HPP
