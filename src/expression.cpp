#include "expression.hpp"

expression::expression(simple_type t, int var, map_boost& hash) : hash_table(hash), t(t), var(var) {
}

// primary expression creation
expression::expression(string s, map_boost& hash) : hash_table(hash) {
	if (hash.find(s) == hash.end()) {
		t = _ERROR;
		var = -1;
		error_funct(_ERROR_COMPIL, s, " was not declared in this scope");
		return;
	}
	else {
		struct identifier &id = hash_table.at(s);
		id.used = true;
		if (id.symbolType != _LOCAL_VAR && id.symbolType != _GLOBAL_VAR) {
			// In this case, we try to laod an expression to an identifier which doesn't accept one
			// For exemple we try to get a value from a function name
			t = _ERROR;
			var = -1;
			error_funct(_ERROR_COMPIL, "Can't load value of ", s);
		}
		else {

			switch (id.t) {
			case _INT:
				t = _INT;
				var = new_var();
				code << "  %x" << var << " = load i32, i32* " << id.name << "\n";
				break;

			case _DOUBLE:
				t = _DOUBLE;
				var = new_var();
				code << "  %x" << var << " = load double, double* " << id.name << "\n";
				break;

			case _VOID:
				error_funct(_ERROR_COMPIL, "Can't load void identifier");
				t = _ERROR;
				var = -1;
				break;

			case _BOOL:
				error_funct(_ERROR_COMPIL, "Can't load bool identifier");
				t = _ERROR;
				var = -1;
				break;

			case _ERROR:
				// In this case, we consider that the expression has already an error and we don't consider others errors
				// in order to don't flood the error output with big expressions.
				t = _ERROR;
				var = -1;
				break;

			default:
				// If you see this something really goes wrong withe the compiler
				error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
				t = _ERROR;
				var = -1;
				break;
			}
		}
	}
}

expression::expression(int i, map_boost& hash) : hash_table(hash), t(_INT), var(new_var()) {
	code << "  %x" << var << " = add i32 0, " << i << "\n";
}

expression::expression(double d, map_boost& hash) : hash_table(hash), t(_DOUBLE), var(new_var()) {
	char* nb_double = double_to_hex_str(d);
	code << "  %x" << var << " = fadd double 0x000000000000000, " << nb_double << "\n";
	free(nb_double);
}


// Function call
expression::expression(char *s, void *, map_boost &hash) : hash_table(hash) { //without parameter
	if (hash.find(s) == hash.end()) {
		t = _ERROR;
		var = -1;
		error_funct(_ERROR_COMPIL, s, " was not declared in this scope");
	}
	else {
		struct identifier &id = hash.at(s);
		if (id.symbolType != _FUNCTION) {
			// In this case, we try to call something that is not a function
			t = _ERROR;
			var = -1;
			error_funct(_ERROR_COMPIL, s, " is not a function");
		} else if (!id.paramTypes.empty()) {
			t = _ERROR;
			var = -1;
			error_funct(_ERROR_COMPIL, "too much arguments to function ", s);
		}
		else {
			id.used = true; //used at the end to determine which of p5 function need to be declared
			t = id.t; //expression type is the function return type
			switch (id.t) {
			// We store result only if the function is a non void return function.
			case _INT:
				var = new_var();
				code << "  %x" << var << " = call i32 ";
				break;
			case _DOUBLE:
				var = new_var();
				code << "  %x" << var << " = call double ";
				break;
			default:
				var = -1;
				code << "  call void ";
				break;
			}
			code << id.name << "()\n";
		}
	}
}


expression::expression(char *s, struct arg_expr_list &ael, map_boost &hash) : hash_table(hash) { //with parameters
	if (hash.find(s) == hash.end()) {
		t = _ERROR;
		var = -1;
		error_funct(_ERROR_COMPIL, s, " was not declared in this scope");
	}
	else {
		struct identifier &id = hash.at(s);
		if (id.symbolType != _FUNCTION) {
			// In this case, we try to call something that is not a function
			t = _ERROR;
			var = -1;
			error_funct(_ERROR_COMPIL, s, " is not a function");
		}
		else if (id.paramTypes.size() < ael.codeV.size()) {
			t = _ERROR;
			var = -1;
			error_funct(_ERROR_COMPIL, "too much arguments to function ", s);
		}
		else if (id.paramTypes.size() > ael.codeV.size()) {
			t = _ERROR;
			var = -1;
			error_funct(_ERROR_COMPIL, "too few arguments to function ", s);
		}
		else {

			id.used = true; //used at the end to determine which of p5 function need to be declared

			int count = 0;
			enum simple_type param_type;
			BOOST_FOREACH(struct expression *expression, ael.codeV) {
				// in this loop we write expression code before function call in order to call it with results of theses expressions
				// We also change register where expression is stored in order to have the good value on the function call

				param_type = id.paramTypes[count];
				count++;
				int newVar;

				// Error gestion : if used in order to avoid repetition in the switch
				if (param_type == _VOID || expression->getT() == _VOID) {
					error_funct(_ERROR_COMPIL, "invalid use of void expression");
					expression->setT(_ERROR);
					t = _ERROR;
					var = -1;
				}
				else if (param_type == _BOOL || expression->getT() == _BOOL) {
					error_funct(_ERROR_COMPIL, "implicit conversion from type 'boolean' to an other type is not allowed");
					expression->setT(_ERROR);
					t = _ERROR;
					var = -1;
				}
				else if (param_type == _ERROR || expression->getT() == _ERROR) {
					// In this case, we consider that the expression has already an error and we don't consider others errors
					// in order to don't flood the error output with big expressions.
					expression->setT(_ERROR);
					t = _ERROR;
					var = -1;
				}
				else {
					switch (param_type) {
					case _INT:
						switch (expression->getT()) {

						case _INT:
							// No conversion needed, expression has the same type as the function parameter
							code << expression->code.str();
							break;

						case _DOUBLE:
							// Need to convert expression from double to int
							newVar = new_var();
							code << expression->code.str();
							code << "  %x" << newVar <<  " = fptosi double %x" << expression->getVar() << " to i32\n";
							expression->setVar(newVar);
							break;

						default:
							expression->setT(_ERROR);
							break;
						}

						break;

					case _DOUBLE:

						switch (expression->getT()) {
						case _INT:
							// Need to convert expression from int to double
							newVar = new_var();
							code << expression->code.str();
							code << "  %x" << newVar << " = sitofp i32 %x" << expression->getVar() << " to double\n";
							expression->setVar(newVar);
							break;

						case _DOUBLE:
							code << expression->code.str();
							break;

						default:
							expression->setT(_ERROR);
							break;
						}

						break;

					default:
						expression->setT(_ERROR);
						break;
					}
				}
			}


			t = id.t; //expression type is the function return type
			switch (id.t) {
			// We store result only if the function is a non void return function.
			case _INT:
				var = new_var();
				code << "  %x" << var << " = call i32 ";
				break;
			case _DOUBLE:
				var = new_var();
				code << "  %x" << var << " = call double ";
				break;
			default:
				var = -1;
				code << "  call void ";
				break;
			}
			code << id.name << "(";

			count = 0;
			int size = ael.codeV.size();
			BOOST_FOREACH(struct expression *expression, ael.codeV) {
				// Here we call function with the good values

				param_type = id.paramTypes[count];
				count++;

				switch (param_type) {

				case _INT:
					code << "i32 %x" << expression->getVar();
					if (count < size) code << ", ";
					break;

				case _DOUBLE:
					code << "double %x" << expression->getVar();
					if (count < size) code << ", ";
					break;

				default:
					// Error gestion was done before
					break;

				}

				delete expression; //memory free
			}

			code << ")\n";

		}
	}
}



// Getters
simple_type expression::getT() const {
	return t;
}

int expression::getVar() const {
	return var;
}

map_boost& expression::getHash() const  {
	return hash_table;
}


// Setters
void expression::setVar(int var) {
	this->var = var;
}

void expression::setT(enum simple_type t) {
	this->t = t;
}

expression::~expression() {
}


// Code factorisation for the generation of binaries operators
struct expression* binary_operator(const struct expression& e1, const struct expression& e2,
								   string integer_op, string double_op,
								   enum simple_type integer_res, enum simple_type double_res) {
	struct expression* ret;
	int newV;

	// Error gestion : if used in order to avoid repetition in the switch
	if (e1.t == _VOID || e2.t == _VOID) {
		error_funct(_ERROR_COMPIL, "invalid use of void expression");
		ret = new expression(_ERROR, -1, e1.hash_table);
	}
	else if (e1.t == _BOOL || e2.t == _BOOL) {
		error_funct(_ERROR_COMPIL, "implicit conversion from type 'boolean' to an other type is not allowed");
		ret = new expression(_ERROR, -1, e1.hash_table);
	}
	else if (e1.t == _ERROR || e2.t == _ERROR) {
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		ret = new expression(_ERROR, -1, e1.hash_table);
	}
	else {
		switch (e1.t) {

		case _INT:
			switch (e2.t) {

			case _INT:
				// No conversion needed, both expression have the same type
				ret = new expression(integer_res, new_var(), e1.hash_table);
				ret->code << e1.code.str() << e2.code.str();
				ret->code << "  %x" << ret->getVar() << " = " << integer_op << " i32 %x" << e1.var << ", %x" << e2.var << "\n";
				break;

			case _DOUBLE:
				// Need to convert e1 from int to double
				newV = new_var();
				ret = new expression(double_res, new_var(), e1.hash_table);
				ret->code << e1.code.str();
				ret->code << "  %x" << newV << " = sitofp i32 %x" << e1.var << " to double\n";
				ret->code << e2.code.str();
				ret->code << "  %x" << ret->getVar() << " = " << double_op << " double %x" << newV << ", %x" << e2.var << "\n";
				break;

			default:
				ret = new expression(_ERROR, -1, e1.hash_table);
				break;

			}
			break;

		case _DOUBLE:
			switch (e2.t) {

			case _INT:
				// Need to convert e2 from int to double
				newV = new_var();
				ret = new expression(double_res, new_var(), e1.hash_table);
				ret->code << e1.code.str() << e2.code.str();
				ret->code << "  %x" << newV << " = sitofp i32 %x" << e2.var << " to double\n";
				ret->code << "  %x" << ret->getVar() << " = " << double_op << " double %x" << e1.var << ", %x" << newV << "\n";
				break;

			case _DOUBLE:
				// No conversion needed, both expression have the same type
				ret = new expression(double_res, new_var(), e1.hash_table);
				ret->code << e1.code.str() << e2.code.str();
				ret->code << "  %x" << ret->getVar() << " = " << double_op << " double %x" << e1.var << ", %x" << e2.var << "\n";
				break;

			default:
				ret = new expression(_ERROR, -1, e1.hash_table);
				break;
			}
			break;

		default:
			ret = new expression(_ERROR, -1, e1.hash_table);
			break;

		}
	}
	return ret;
}

// Code generation for basics operations between expressions
struct expression* operator+(const struct expression& e1, const struct expression& e2) {
	return binary_operator(e1, e2, "add", "fadd", _INT, _DOUBLE);
}

struct expression* operator-(const struct expression& e1, const struct expression& e2) {
	return binary_operator(e1, e2, "sub", "fsub", _INT, _DOUBLE);
}

struct expression* operator*(const struct expression& e1, const struct expression& e2) {
	return binary_operator(e1, e2, "mul", "fmul", _INT, _DOUBLE);
}

struct expression* operator/(const struct expression& e1, const struct expression& e2) {
	return binary_operator(e1, e2, "sdiv", "fdiv", _INT, _DOUBLE);
}

struct expression* operator%(const struct expression& e1, const struct expression& e2) {
	return binary_operator(e1, e2, "srem", "frem", _INT, _DOUBLE);
}



struct expression* operator<<(const struct expression& e1, const struct expression& e2) {
	//This operation is only permitted if e1 is an integers.
	//We allow implicit conversion for e2 from double to int
	struct expression* ret;

	// Error gestion : if used in order to avoid repetition in the switch
	if (e1.t == _VOID || e2.t == _VOID) {
		error_funct(_ERROR_COMPIL, "invalid use of void expression");
		ret = new expression(_ERROR, -1, e1.hash_table);
	}
	else if (e1.t == _BOOL || e2.t == _BOOL) {
		error_funct(_ERROR_COMPIL, "implicit conversion from type 'boolean' to an other type is not allowed");
		ret = new expression(_ERROR, -1, e1.hash_table);
	}
	else if (e1.t == _ERROR || e2.t == _ERROR) {
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		ret = new expression(_ERROR, -1, e1.hash_table);
	}
	else {
		int conversion;
		switch (e1.t) {
		case _INT:

			switch (e2.t) {

			case _INT:
				ret = new expression(_INT, new_var(), e1.hash_table);
				ret->code << e1.code.str() << e2.code.str();
				ret->code << "%x" << ret->getVar() << " = shl i32 %x" << e1.var << ", %x" << e2.var << "\n";
				break;

			case _DOUBLE:
				// Need to convert e2 from double to int
				conversion = new_var();
				ret = new expression(_INT, new_var(), e1.hash_table);
				ret->code << e1.code.str() << e2.code.str();
				ret->code << "%x" << conversion <<  " = fptosi double %x" << e2.var << " to i32\n";
				ret->code << "%x" << ret->getVar() << " = shl i32 %x" << e1.var << ", %x" << conversion << "\n";
				break;

			default:
				ret = new expression(_ERROR, -1, e1.hash_table);
				break;
			}

			break;

		case _DOUBLE:
			error_funct(_ERROR_COMPIL, "SHL operation canno't be applied to a double expression");
			ret = new expression(_ERROR, -1, e1.hash_table);
			break;

		default:
			ret = new expression(_ERROR, -1, e1.hash_table);
			break;
		}
	}
	return ret;
}

struct expression* operator>>(const struct expression& e1, const struct expression& e2) {
	//This operation is only permitted if e1 is an integers.
	//We allow implicit conversion for e2 from double to int
	struct expression* ret;

	// Error gestion : if used in order to avoid repetition in the switch
	if (e1.t == _VOID || e2.t == _VOID) {
		error_funct(_ERROR_COMPIL, "invalid use of void expression");
		ret = new expression(_ERROR, -1, e1.hash_table);
	}
	else if (e1.t == _BOOL || e2.t == _BOOL) {
		error_funct(_ERROR_COMPIL, "implicit conversion from type 'boolean' to an other type is not allowed");
		ret = new expression(_ERROR, -1, e1.hash_table);
	}
	else if (e1.t == _ERROR || e2.t == _ERROR) {
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		ret = new expression(_ERROR, -1, e1.hash_table);
	}
	else {
		int conversion;
		switch (e1.t) {
		case _INT:

			switch (e2.t) {

			case _INT:
				ret = new expression(_INT, new_var(), e1.hash_table);
				ret->code << e1.code.str() << e2.code.str();
				ret->code << "%x" << ret->getVar() << " = ashr i32 %x" << e1.var << ", %x" << e2.var << "\n";
				break;

			case _DOUBLE:
				// Need to convert e2 from double to int
				conversion = new_var();
				ret = new expression(_INT, new_var(), e1.hash_table);
				ret->code << e1.code.str() << e2.code.str();
				ret->code << "%x" << conversion <<  " = fptosi double %x" << e2.var << " to i32\n";
				ret->code << "%x" << ret->getVar() << " = ashr i32 %x" << e1.var << ", %x" << conversion << "\n";
				break;

			default:
				ret = new expression(_ERROR, -1, e1.hash_table);
				break;
			}

			break;

		case _DOUBLE:
			error_funct(_ERROR_COMPIL, "SHR operation canno't be applied to a double expression");
			ret = new expression(_ERROR, -1, e1.hash_table);
			break;

		default:
			ret = new expression(_ERROR, -1, e1.hash_table);
			break;
		}
	}
	return ret;
}


// Code generation for assignements
struct expression* expression::operator=(string s) {
	struct expression* ret;

	if (hash_table.find(s) == hash_table.end()) {
		ret = new expression(_ERROR, -1, hash_table);
		error_funct(_ERROR_COMPIL, s, " was not declared in this scope");
	}

	else {
		struct identifier &id = hash_table.at(s);
		id.used = true;
		if (id.symbolType != _LOCAL_VAR && id.symbolType != _GLOBAL_VAR) {
			// In this case, we try to assign an expression to an identifier which doesn't accept one
			// For exemple we try to assign an expression to a function name
			ret = new expression(_ERROR, -1, hash_table);
			error_funct(_ERROR_COMPIL, "Can't assign an expression to ", s);
		}
		else {
			int newVar = var;

			// Error gestion : if used in order to avoid repetition in the switch
			if (t == _VOID || id.t == _VOID) {
				error_funct(_ERROR_COMPIL, "invalid use of void expression");
				t = _ERROR;
				var = -1;
			}
			else if (t == _BOOL || id.t == _BOOL) {
				error_funct(_ERROR_COMPIL, "implicit conversion from type 'boolean' to an other type is not allowed");
				t = _ERROR;
				var = -1;
			}
			else if (t == _ERROR || id.t == _ERROR) {
				// In this case, we consider that the expression has already an error and we don't consider others errors
				// in order to don't flood the error output with big expressions.
				t = _ERROR;
				var = -1;
			}
			else {
				switch (id.t) {

				case _INT:
					switch (t) {

					case _INT:
						// No conversion needed
						ret = new expression(_INT, var, hash_table);
						ret->code << code.str();
						ret->code << "  store i32 %x" << var << ", i32* " << id.name << "\n";
						break;

					case _DOUBLE:
						// Need to convert expression from double to int
						newVar = new_var();
						ret = new expression(_INT, newVar, hash_table);
						ret->code << code.str();
						ret->code << "  %x" << newVar <<  " = fptosi double %x" << var << " to i32\n";
						ret->code << "  store i32 %x" << newVar << ", i32* " << id.name << "\n";
						break;

					default:
						t = _ERROR;
						var = -1;
						break;

					}
					break;

				case _DOUBLE:
					switch (t) {
					case _INT:
						// Need to convert expression from int to double
						newVar = new_var();
						ret = new expression(_DOUBLE, newVar, hash_table);
						ret->code << code.str();
						ret->code << "  %x" << newVar << " = sitofp i32 %x" << var << " to double\n";
						ret->code << "  store double %x" << newVar << ", double* " << id.name << "\n";
						break;

					case _DOUBLE:
						// No conversion needed
						ret = new expression(_DOUBLE, var, hash_table);
						ret->code << code.str();
						ret->code << "  store double %x" << var << ", double* " << id.name << "\n";
						break;

					default:
						ret = new expression(_ERROR, -1, hash_table);
						break;

					}
					break;

				default:
					t = _ERROR;
					var = -1;
					break;
				}
			}
		}
	}
	return ret;
}

struct expression* expression::operator+=(string s) {
	struct expression* ret;
	struct expression* e1 = new expression(s, hash_table);
	struct expression* e2 = *e1 + *this;
	ret = (*e2 = s);
	delete e1; e1 = nullptr; delete e2; e2 = nullptr;
	return ret;
}

struct expression* expression::operator-=(string s) {
	struct expression* ret;
	struct expression* e1 = new expression(s, hash_table);
	struct expression* e2 = *e1 - *this;
	ret = (*e2 = s);
	delete e1; e1 = nullptr; delete e2; e2 = nullptr;
	return ret;
}

struct expression* expression::operator*=(string s) {
	struct expression* ret;
	struct expression* e1 = new expression(s, hash_table);
	struct expression* e2 = *e1** this;
	ret = (*e2 = s);
	delete e1; e1 = nullptr; delete e2; e2 = nullptr;
	return ret;
}

struct expression* expression::operator/=(string s) {
	struct expression* ret;
	struct expression* e1 = new expression(s, hash_table);
	struct expression* e2 = *e1 / *this;
	ret = (*e2 = s);
	delete e1; e1 = nullptr; delete e2; e2 = nullptr;
	return ret;
}

struct expression* expression::operator%=(string s) {
	struct expression* ret;
	struct expression* e1 = new expression(s, hash_table);
	struct expression* e2 = *e1 % *this;
	ret = (*e2 = s);
	delete e1; e1 = nullptr; delete e2; e2 = nullptr;
	return ret;
}

struct expression* expression::operator<<=(string s) {
	struct expression* ret;
	struct expression* e1 = new expression(s, hash_table);
	struct expression* e2 = *e1 << *this;
	ret = (*e2 = s);
	delete e1; e1 = nullptr; delete e2; e2 = nullptr;
	return ret;
}

struct expression* expression::operator>>=(string s) {
	struct expression* ret;
	struct expression* e1 = new expression(s, hash_table);
	struct expression* e2 = *e1 >> *this;
	ret = (*e2 = s);
	delete e1; e1 = nullptr; delete e2; e2 = nullptr;
	return ret;
}


// Code generation for conditionals expression
struct expression* operator==(const struct expression& e1, const struct expression& e2) {
	return binary_operator(e1, e2, "icmp eq", "fcmp oeq", _BOOL, _BOOL);
}

struct expression* operator!=(const struct expression& e1, const struct expression& e2) {
	return binary_operator(e1, e2, "icmp ne", "fcmp one", _BOOL, _BOOL);
}

struct expression* operator<(const struct expression& e1, const struct expression& e2) {
	return binary_operator(e1, e2, "icmp slt", "fcmp olt", _BOOL, _BOOL);
}

struct expression* operator>(const struct expression& e1, const struct expression& e2) {
	return binary_operator(e1, e2, "icmp sgt", "fcmp ogt", _BOOL, _BOOL);
}

struct expression* operator<=(const struct expression& e1, const struct expression& e2) {
	return binary_operator(e1, e2, "icmp sle", "fcmp ole", _BOOL, _BOOL);
}

struct expression* operator>=(const struct expression& e1, const struct expression& e2) {
	return binary_operator(e1, e2, "icmp sge", "fcmp oge", _BOOL, _BOOL);
}


// Code generation for logicales operators
struct expression* operator&&(const struct expression& e1, const struct expression& e2) {
	struct expression* ret;

	// Error gestion : if used in order to avoid repetition in the switch
	if (e1.t == _VOID || e2.t == _VOID) {
		error_funct(_ERROR_COMPIL, "invalid use of void expression");
		ret = new expression(_ERROR, -1, e1.hash_table);
	}
	else if (e1.t == _INT || e1.t == _DOUBLE || e2.t == _INT || e2.t == _DOUBLE) {
		error_funct(_ERROR_COMPIL, "Can't apply a logical operator between booleans and other types");
		ret = new expression(_ERROR, -1, e1.hash_table);
	}
	else if (e1.t == _ERROR || e2.t == _ERROR) {
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		ret = new expression(_ERROR, -1, e1.hash_table);
	}
	else {
		switch (e1.t) {
		case _BOOL:

			switch (e2.t) {

			case _BOOL:
				ret = new expression(_BOOL, new_var(), e1.hash_table);
				ret->code << e1.code.str() << e2.code.str();
				ret->code << "%x" << ret->getVar() << " = and i1 %x" << e1.var << ", %x" << e2.var << "\n";
				break;

			default:
				ret = new expression(_ERROR, -1, e1.hash_table);
				break;
			}

			break;

		default:
			ret = new expression(_ERROR, -1, e1.hash_table);
			break;
		}
	}
	return ret;
}

struct expression* operator||(const struct expression& e1, const struct expression& e2) {
	struct expression* ret;

	// Error gestion : if used in order to avoid repetition in the switch
	if (e1.t == _VOID || e2.t == _VOID) {
		error_funct(_ERROR_COMPIL, "invalid use of void expression");
		ret = new expression(_ERROR, -1, e1.hash_table);
	}
	else if (e1.t == _INT || e1.t == _DOUBLE || e2.t == _INT || e2.t == _DOUBLE) {
		error_funct(_ERROR_COMPIL, "Can't apply a logical operator between booleans and other types");
		ret = new expression(_ERROR, -1, e1.hash_table);
	}
	else if (e1.t == _ERROR || e2.t == _ERROR) {
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		ret = new expression(_ERROR, -1, e1.hash_table);
	}
	else {
		switch (e1.t) {
		case _BOOL:

			switch (e2.t) {

			case _BOOL:
				ret = new expression(_BOOL, new_var(), e1.hash_table);
				ret->code << e1.code.str() << e2.code.str();
				ret->code << "%x" << ret->getVar() << " = or i1 %x" << e1.var << ", %x" << e2.var << "\n";
				break;

			default:
				ret = new expression(_ERROR, -1, e1.hash_table);
				break;
			}

			break;

		default:
			ret = new expression(_ERROR, -1, e1.hash_table);
			break;
		}
	}
	return ret;
}

struct expression* operator!(const struct expression& e1) { // ! is equivalent to xor with 1
	struct expression* ret;

	// Error gestion : if used in order to avoid repetition in the switch
	if (e1.t == _VOID) {
		error_funct(_ERROR_COMPIL, "invalid use of void expression");
		ret = new expression(_ERROR, -1, e1.hash_table);
	}
	else if (e1.t == _INT || e1.t == _DOUBLE) {
		error_funct(_ERROR_COMPIL, "Can't apply a logical operator between booleans and other types");
		ret = new expression(_ERROR, -1, e1.hash_table);
	}
	else if (e1.t == _ERROR) {
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		ret = new expression(_ERROR, -1, e1.hash_table);
	}
	else {
		switch (e1.t) {
		case _BOOL:
			ret = new expression(_BOOL, new_var(), e1.hash_table);
			ret->code << e1.code.str();
			ret->code << "%x" << ret->getVar() << " = xor i1 %x" << e1.var << ", 1\n";
			break;

		default:
			ret = new expression(_ERROR, -1, e1.hash_table);
			break;
		}
	}
	return ret;
}


// unary operators expressions
struct expression *incr_postfix(string name, map_boost &hash_table) {
	struct expression *ret;
	struct expression e1(name, hash_table);

	int var;
	int backup = e1.getVar();
	switch (e1.getT()) {
	case _INT:
		var = new_var();
		e1.code << "  %x" << var << " = add i32 %x" << backup << ", 1\n";
		e1.setVar(var);
		ret = (e1 = name);
		ret->setVar(backup); //Expression result is the identifier value before the unary operator
		break;

	case _DOUBLE:
		var = new_var();
		e1.code << "  %x" << var << " = fadd double %x" << backup << ", 0x3ff0000000000000\n";
		e1.setVar(var);
		ret = (e1 = name);
		ret->setVar(backup); //Expression result is the identifier value before the unary operator
		break;

	case _VOID:
		error_funct(_ERROR_COMPIL, "invalid use of void expression");
		ret = new expression(_ERROR, -1, hash_table);
		break;

	case _BOOL:
		error_funct(_ERROR_COMPIL, "implicit conversion from type 'boolean' to an other type is not allowed");
		ret = new expression(_ERROR, -1, hash_table);
		break;

	case _ERROR:
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		ret = new expression(_ERROR, -1, hash_table);
		break;

	default:
		// If you see this something really goes wrong withe the compiler
		error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
		ret = new expression(_ERROR, -1, hash_table);
		break;
	}
	return ret;
}

struct expression *decr_postfix(string name, map_boost &hash_table) {
	struct expression *ret;
	struct expression e1(name, hash_table);

	int var;
	int backup = e1.getVar();
	switch (e1.getT()) {
	case _INT:
		var = new_var();
		e1.code << "  %x" << var << " = sub i32 %x" << backup << ", 1\n";
		e1.setVar(var);
		ret = (e1 = name);
		ret->setVar(backup); //Expression result is the identifier value before the unary operator
		break;

	case _DOUBLE:
		var = new_var();
		e1.code << "  %x" << var << " = fsub double %x" << backup << ", 0x3ff0000000000000\n";
		e1.setVar(var);
		ret = (e1 = name);
		ret->setVar(backup); //Expression result is the identifier value before the unary operator
		break;

	case _VOID:
		error_funct(_ERROR_COMPIL, "invalid use of void expression");
		ret = new expression(_ERROR, -1, hash_table);
		break;

	case _BOOL:
		error_funct(_ERROR_COMPIL, "implicit conversion from type 'boolean' to an other type is not allowed");
		ret = new expression(_ERROR, -1, hash_table);
		break;

	case _ERROR:
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		ret = new expression(_ERROR, -1, hash_table);
		break;

	default:
		// If you see this something really goes wrong withe the compiler
		error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
		ret = new expression(_ERROR, -1, hash_table);
		break;
	}
	return ret;
}

struct expression *incr_prefix(string name, map_boost &hash_table) {
	struct expression *ret;
	struct expression e1(name, hash_table);

	int var;
	int backup = e1.getVar();
	switch (e1.getT()) {
	case _INT:
		var = new_var();
		e1.code << "  %x" << var << " = add i32 %x" << backup << ", 1\n";
		e1.setVar(var);
		ret = (e1 = name);
		break;

	case _DOUBLE:
		var = new_var();
		e1.code << "  %x" << var << " = fadd double %x" << backup << ", 0x3ff0000000000000\n";
		e1.setVar(var);
		ret = (e1 = name);
		break;

	case _VOID:
		error_funct(_ERROR_COMPIL, "invalid use of void expression");
		ret = new expression(_ERROR, -1, hash_table);
		break;

	case _BOOL:
		error_funct(_ERROR_COMPIL, "implicit conversion from type 'boolean' to an other type is not allowed");
		ret = new expression(_ERROR, -1, hash_table);
		break;

	case _ERROR:
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		ret = new expression(_ERROR, -1, hash_table);
		break;

	default:
		// If you see this something really goes wrong withe the compiler
		error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
		ret = new expression(_ERROR, -1, hash_table);
		break;
	}
	return ret;
}

struct expression *decr_prefix(string name, map_boost &hash_table) {
	struct expression *ret;
	struct expression e1(name, hash_table);

	int var;
	int backup = e1.getVar();
	switch (e1.getT()) {
	case _INT:
		var = new_var();
		e1.code << "  %x" << var << " = sub i32 %x" << backup << ", 1\n";
		e1.setVar(var);
		ret = (e1 = name);
		break;

	case _DOUBLE:
		var = new_var();
		e1.code << "  %x" << var << " = fsub double %x" << backup << ", 0x3ff0000000000000\n";
		e1.setVar(var);
		ret = (e1 = name);
		break;

	case _VOID:
		error_funct(_ERROR_COMPIL, "invalid use of void expression");
		ret = new expression(_ERROR, -1, hash_table);
		break;

	case _BOOL:
		error_funct(_ERROR_COMPIL, "implicit conversion from type 'boolean' to an other type is not allowed");
		ret = new expression(_ERROR, -1, hash_table);
		break;

	case _ERROR:
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		ret = new expression(_ERROR, -1, hash_table);
		break;

	default:
		// If you see this something really goes wrong withe the compiler
		error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
		ret = new expression(_ERROR, -1, hash_table);
		break;
	}
	return ret;
}

struct expression *opposite(const struct expression &e1){
	struct expression *ret;

	switch (e1.getT()) {
	case _INT:
		ret = new expression(_INT, new_var(), e1.hash_table);
		ret->code << e1.code.str();
		ret->code << "  %x" << ret->getVar() << " = sub i32 0, %x" << e1.getVar() << "\n";
		break;

	case _DOUBLE:
		ret = new expression(_DOUBLE, new_var(), e1.hash_table);
		ret->code << e1.code.str();
		ret->code << "  %x" << ret->getVar() << " = fsub double 0x000000000000000, %x" << e1.getVar() << "\n";
		break;

	case _VOID:
		error_funct(_ERROR_COMPIL, "invalid use of void expression");
		ret = new expression(_ERROR, -1, e1.hash_table);
		break;

	case _BOOL:
		error_funct(_ERROR_COMPIL, "implicit conversion from type 'boolean' to an other type is not allowed");
		ret = new expression(_ERROR, -1, e1.hash_table);
		break;

	case _ERROR:
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		ret = new expression(_ERROR, -1, e1.hash_table);
		break;

	default:
		// If you see this something really goes wrong withe the compiler
		error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
		ret = new expression(_ERROR, -1, e1.hash_table);
		break;
	}
	return ret;
}
