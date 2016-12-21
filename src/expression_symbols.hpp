#ifndef __EXPRESSION_SYMBOLS_H_
#define __EXPRESSION_SYMBOLS_H_

#define SIZE 1013

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include <boost/unordered_map.hpp>
typedef boost::unordered_map<std::string, struct identifier> map_boost;

#include "enum.h"

using namespace std;

struct code_container
{
	stringstream code;
};

struct declarator
{
private:
	vector<string> names;

public:
	declarator(){}
	void add(char *);
	void add(string);
	void show();
	void merge(declarator *other);

	std::vector<string>::iterator begin();
	std::vector<string>::iterator end();
};

struct identifier {
	enum simple_type t;
	int test;
	string name;
	int register_no;
};

#endif // __EXPRESSION_SYMBOLS_H_
