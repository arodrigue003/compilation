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

// used for function declaration
struct code_container* declare_funct(enum simple_type t, string name, map_list &ref_tab);
struct code_container* declare_funct(enum simple_type t, string name, struct type_list tl, map_list &ref_tab);

// declaration
struct code_container* local_declaration(enum simple_type t, struct declaration_list &decla, map_list &ref_tab);
struct code_container* global_declaration(enum simple_type t, struct declaration_list &decla, map_list &ref_tab);

// return
struct code_container *return_statement(struct expression& e1);

// Function definition
struct code_container *define_funct(enum simple_type t, string name, map_list &ref_tab);

#endif // STATEMENT_HPP
