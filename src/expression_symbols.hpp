#ifndef __EXPRESSION_SYMBOLS_H_
#define __EXPRESSION_SYMBOLS_H_

#define SIZE 1013

#include <string>
#include <iostream>
#include <sstream>
#include <vector>

#include "enum.h"

using namespace std;


struct expression
{
private:
    enum simple_type t;
    int var;

public:
	char *old;
	char *indentifier;
	stringstream code;

    expression(simple_type t, int var);
    virtual ~expression();

    simple_type getT() const;
    int getVar() const;
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
};

#endif // __EXPRESSION_SYMBOLS_H_
