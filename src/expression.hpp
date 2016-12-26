#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include "utilityFunctions.hpp"

struct expression : public code_container {
private:
	map_boost& hash_table;
	enum simple_type t;
	int var;

public:

	// primary expression creation
	expression(string s, map_boost& hash);
	expression(int i, map_boost& hash);
	expression(double d, map_boost& hash);
	expression(simple_type t, int var, map_boost& hash);

	// Function call
	expression(char *s, void *, map_boost &hash);
	expression(char *s, struct arg_expr_list &ael, map_boost &hash);

	virtual ~expression();

	// Getters
	simple_type getT() const;
	int getVar() const;
	map_boost& getHash() const;

	// Settes
	void setVar(int var);
	void setT(enum simple_type t);

	// Code factorisation for the generation of binaries operators
	friend struct expression* binary_operator(const struct expression& e1, const struct expression& e2,
											  string integer_op, string double_op,
											  enum simple_type integer_res, enum simple_type double_res);

	// Code generation for binary operators
	friend struct expression* operator+(const struct expression& e1, const struct expression& e2);
	friend struct expression* operator-(const struct expression& e1, const struct expression& e2);
	friend struct expression* operator*(const struct expression& e1, const struct expression& e2);
	friend struct expression* operator/(const struct expression& e1, const struct expression& e2);
	friend struct expression* operator%(const struct expression& e1, const struct expression& e2);
	friend struct expression* operator<<(const struct expression& e1, const struct expression& e2);
	friend struct expression* operator>>(const struct expression& e1, const struct expression& e2);

	// Code generation for assignments
	struct expression* operator=(string s);
	struct expression* operator+=(string s);
	struct expression* operator-=(string s);
	struct expression* operator*=(string s);
	struct expression* operator/=(string s);
	struct expression* operator%=(string s);
	struct expression* operator<<=(string s);
	struct expression* operator>>=(string s);

	// Code generation for conditionals expression
	friend struct expression* operator==(const struct expression& e1, const struct expression& e2);
	friend struct expression* operator!=(const struct expression& e1, const struct expression& e2);
	friend struct expression* operator<(const struct expression& e1, const struct expression& e2);
	friend struct expression* operator>(const struct expression& e1, const struct expression& e2);
	friend struct expression* operator<=(const struct expression& e1, const struct expression& e2);
	friend struct expression* operator>=(const struct expression& e1, const struct expression& e2);

	// Code generation for logicales operators
	friend struct expression* operator&&(struct expression& e1, const struct expression& e2);
	friend struct expression* operator||(const struct expression& e1, const struct expression& e2);
	friend struct expression* operator!(const struct expression& e1);

	friend struct expression *opposite(const struct expression &e1);
};


// unary operators expressions
struct expression *incr_postfix(string name, map_boost &hash_table);
struct expression *decr_postfix(string name, map_boost &hash_table);
struct expression *incr_prefix(string name, map_boost &hash_table);
struct expression *decr_prefix(string name, map_boost &hash_table);
struct expression *opposite(const struct expression &e1);


struct arg_expr_list{
	vector<expression *> codeV;
};

#endif // EXPRESSION_HPP
