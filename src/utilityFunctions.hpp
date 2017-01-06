#ifndef UTILITYFUNCTIONS_HPP
#define UTILITYFUNCTIONS_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <list>

using namespace std;

#include <boost/foreach.hpp>
#include <boost/unordered_map.hpp>
typedef boost::unordered_map<std::string, struct identifier> map_boost;
typedef list<map_boost> map_list;

extern bool has_error;

#include "enum.h"

int yyerror(const char *s);

// some struct definitions for yack
struct code_container {
    stringstream code;
	bool has_return = false;
};

struct identifier {
    enum simple_type t;
    string name;
    int register_no;
    enum identifier_type symbolType;
	bool from_q5 = false;
	bool used = false;

    vector<enum simple_type> paramTypes; //used for functions identification

public:
	identifier(){}

	// used for q5 declaration
	identifier(enum simple_type t, string name) : t(t), name(name), symbolType(_FUNCTION), from_q5(true) {}
	identifier(enum simple_type t, string name, enum simple_type p1): t(t), name(name), symbolType(_FUNCTION), from_q5(true) {
		paramTypes.push_back(p1);
	}
	identifier(enum simple_type t, string name, enum simple_type p1, enum simple_type p2): t(t), name(name), symbolType(_FUNCTION), from_q5(true) {
		paramTypes.push_back(p1);
		paramTypes.push_back(p2);
	}
	identifier(enum simple_type t, string name, enum simple_type p1, enum simple_type p2, enum simple_type p3): t(t), name(name), symbolType(_FUNCTION), from_q5(true) {
		paramTypes.push_back(p1);
		paramTypes.push_back(p2);
		paramTypes.push_back(p3);
	}
	identifier(enum simple_type t, string name, enum simple_type p1, enum simple_type p2, enum simple_type p3, enum simple_type p4): t(t), name(name), symbolType(_FUNCTION), from_q5(true) {
		paramTypes.push_back(p1);
		paramTypes.push_back(p2);
		paramTypes.push_back(p3);
		paramTypes.push_back(p4);
	}
};

struct declaration_list : public code_container {
    vector<identifier> idList;

public:
	declaration_list(enum simple_type t, string name, map_list &ref_tab);
	declaration_list() : code_container(){}
};

struct type_list {
	vector<enum simple_type> list;
};


// registers and labels counters
int new_var();
int new_label();

void add_identifier(vector<identifier> &to_store, stringstream& ss); //TODO changer cette merde

char* double_to_hex_str(double d);

// Identifier gestion
struct expression* load_identifier(char* s, map_boost hash);

//Error function
int error_funct (enum error_type et, string s);
int error_funct(enum error_type et, string s1, string s2);

//Function to add p5 functions to the hash table in order to recognize them
void setup_p5(map_boost &hash);
struct code_container* declare_q5_used_functions(map_list &ref_tab);


#endif // UTILITYFUNCTIONS_HPP
