#ifndef __EXPRESSION_SYMBOLS_H_
#define __EXPRESSION_SYMBOLS_H_

#define SIZE 1013

#include <string>
#include <iostream>
#include <sstream>

using namespace std;

enum simple_type
{
    ENTIER,
    _DOUBLE,
    _INT
};

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

#endif // __EXPRESSION_SYMBOLS_H_
