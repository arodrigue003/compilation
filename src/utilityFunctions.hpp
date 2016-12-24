#ifndef UTILITYFUNCTIONS_HPP
#define UTILITYFUNCTIONS_HPP

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

#include <boost/unordered_map.hpp>
typedef boost::unordered_map<std::string, struct identifier> map_boost;

#include "enum.h"

// some struct definitions for yack
struct code_container {
	stringstream code;
};

struct identifier {
	enum simple_type t;
	string name;
	int register_no;
	enum identifier_type symbolType;

	vector<enum simple_type> paramTypes; //used for functions identification
};

struct declaration_list : public code_container{
	vector<identifier> idList;
};



// registers and labels counters
int new_var();
int new_label();

void add_identifier(vector<identifier> to_store, stringstream& ss); //TODO changer cette merde

char* double_to_hex_str(double d);

// Identifier gestion
struct expression* load_identifier(char* s, map_boost hash);

#endif // UTILITYFUNCTIONS_HPP
