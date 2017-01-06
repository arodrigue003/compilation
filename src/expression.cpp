#include "expression.hpp"

expression::expression(simple_type t, int var, map_list& ref_tab) : ref_tab(ref_tab), t(t), var(var) {
}

// primary expression creation
expression::expression(string s, map_list& ref_tab) : ref_tab(ref_tab) {

	list<map_boost>::iterator it;
	bool error = true;
	for(it=ref_tab.begin(); it != ref_tab.end(); ++it) {
		if ((*it).find(s) != (*it).end()) {
			error = false;
			goto end;
		}
	}
end:

	if (error) {
		t = _ERROR;
		var = -1;
		code << "error";
		error_funct(_ERROR_COMPIL, s, " was not declared in this scope");
		return;
	}
	else {
		map_boost& hash = *it;
		struct identifier &id = hash.at(s);
		id.used = true;
		if (id.symbolType != _LOCAL_VAR && id.symbolType != _GLOBAL_VAR) {
			// In this case, we try to laod an expression to an identifier which doesn't accept one
			// For exemple we try to get a value from a function name
			t = _ERROR;
			var = -1;
			code << "error";
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

expression::expression(int i, map_list& ref_tab) : ref_tab(ref_tab), t(_INT), var(-1), primary_expr(true), int_val(i) {
	code << "  %x" << var << " = add i32 0, " << i << "\n";
}

expression::expression(double d, map_list& ref_tab) : ref_tab(ref_tab), t(_DOUBLE), var(-1), primary_expr(true), double_val(d) {
	char* nb_double = double_to_hex_str(d);
	code << "  %x" << var << " = fadd double 0x000000000000000, " << nb_double << "\n";
	free(nb_double);
}


// Function call
expression::expression(char *s, void *, map_list& ref_tab) : ref_tab(ref_tab) { //without parameter

	list<map_boost>::iterator it;
	bool error = true;
	for(it=ref_tab.begin(); it != ref_tab.end(); ++it) {
		if ((*it).find(s) != (*it).end()) {
			error = false;
			goto end;
		}
	}
end:

	if (error) {
		t = _ERROR;
		var = -1;
		code << "error";
		error_funct(_ERROR_COMPIL, s, " was not declared in this scope");
	}
	else {
		map_boost& hash = *it;
		struct identifier &id = hash.at(s);
		if (id.symbolType != _FUNCTION) {
			// In this case, we try to call something that is not a function
			t = _ERROR;
			var = -1;
			code << "error";
			error_funct(_ERROR_COMPIL, s, " is not a function");
		} else if (!id.paramTypes.empty()) {
			t = _ERROR;
			var = -1;
			code << "error";
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

expression::expression(char *s, struct arg_expr_list &ael, map_list& ref_tab) : ref_tab(ref_tab) { //with parameters

	list<map_boost>::iterator it;
	bool error = true;
	for(it=ref_tab.begin(); it != ref_tab.end(); ++it) {
		if ((*it).find(s) != (*it).end()) {
			error = false;
			goto end;
		}
	}
end:

	if (error) {
		t = _ERROR;
		var = -1;
		code << "error";
		error_funct(_ERROR_COMPIL, s, " was not declared in this scope");
	}
	else {
		map_boost& hash = *it;
		struct identifier &id = hash.at(s);
		if (id.symbolType != _FUNCTION) {
			// In this case, we try to call something that is not a function
			t = _ERROR;
			var = -1;
			code << "error";
			error_funct(_ERROR_COMPIL, s, " is not a function");
		}
		else if (id.paramTypes.size() < ael.codeV.size()) {
			t = _ERROR;
			var = -1;
			code << "error";
			error_funct(_ERROR_COMPIL, "too much arguments to function ", s);
		}
		else if (id.paramTypes.size() > ael.codeV.size()) {
			t = _ERROR;
			var = -1;
			code << "error";
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
					//We don't get code of the value is immediate
					if (!expression->getPrimaryExpr()) {
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
				char *nb_double;

				switch (param_type) {

				case _INT:
					// We can get the imediate value here
					if (expression->getPrimaryExpr())
						code << "i32 " << expression->getIntVal();
					else
						code << "i32 %x" << expression->getVar();
					if (count < size) code << ", ";
					break;

				case _DOUBLE:
					// We can get the imediate value here
					if (expression->getPrimaryExpr()) {
						nb_double = double_to_hex_str(expression->getDoubleVal());
						code << "double " << nb_double;
						free(nb_double);
					}
					else
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

map_list& expression::getHash() const  {
	return ref_tab;
}

int expression::getIntVal() const {
	return int_val;
}

double expression::getDoubleVal() const {
	return double_val;
}

bool expression::getPrimaryExpr() const {
	return primary_expr;
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
		ret = new expression(_ERROR, -1, e1.ref_tab);
	}
	else if (e1.t == _BOOL || e2.t == _BOOL) {
		error_funct(_ERROR_COMPIL, "implicit conversion from type 'boolean' to an other type is not allowed");
		ret = new expression(_ERROR, -1, e1.ref_tab);
	}
	else if (e1.t == _ERROR || e2.t == _ERROR) {
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		ret = new expression(_ERROR, -1, e1.ref_tab);
	}
	else {

		switch (e1.t) {

		case _INT:
			switch (e2.t) {

			case _INT:
				// No conversion needed, both expression have the same type
				ret = new expression(integer_res, new_var(), e1.ref_tab);
				ret->code << e1.code.str() << e2.code.str();
				ret->code << "  %x" << ret->getVar() << " = " << integer_op << " i32 %x" << e1.var << ", %x" << e2.var << "\n";
				break;

			case _DOUBLE:
				// Need to convert e1 from int to double
				newV = new_var();
				ret = new expression(double_res, new_var(), e1.ref_tab);
				ret->code << e1.code.str();
				ret->code << "  %x" << newV << " = sitofp i32 %x" << e1.var << " to double\n";
				ret->code << e2.code.str();
				ret->code << "  %x" << ret->getVar() << " = " << double_op << " double %x" << newV << ", %x" << e2.var << "\n";
				break;

			default:
				ret = new expression(_ERROR, -1, e1.ref_tab);
				break;

			}
			break;

		case _DOUBLE:
			switch (e2.t) {

			case _INT:
				// Need to convert e2 from int to double
				newV = new_var();
				ret = new expression(double_res, new_var(), e1.ref_tab);
				ret->code << e1.code.str() << e2.code.str();
				ret->code << "  %x" << newV << " = sitofp i32 %x" << e2.var << " to double\n";
				ret->code << "  %x" << ret->getVar() << " = " << double_op << " double %x" << e1.var << ", %x" << newV << "\n";
				break;

			case _DOUBLE:
				// No conversion needed, both expression have the same type
				ret = new expression(double_res, new_var(), e1.ref_tab);
				ret->code << e1.code.str() << e2.code.str();
				ret->code << "  %x" << ret->getVar() << " = " << double_op << " double %x" << e1.var << ", %x" << e2.var << "\n";
				break;

			default:
				ret = new expression(_ERROR, -1, e1.ref_tab);
				break;
			}
			break;

		default:
			ret = new expression(_ERROR, -1, e1.ref_tab);
			break;

		}
	}
	return ret;
}

// optimisation
struct expression* optimize(const struct expression& e1, const struct expression& e2,
							string integer_op, string double_op,
							auto int_func, auto double_func,
							enum simple_type integer_res = _INT, enum simple_type double_res = _DOUBLE) {
	struct expression* ret = NULL;

	// Error gestion : if used in order to avoid repetition in the switch
	if (e1.t == _VOID || e2.t == _VOID) {
		error_funct(_ERROR_COMPIL, "invalid use of void expression");
		ret = new expression(_ERROR, -1, e1.ref_tab);
	}
	else if (e1.t == _BOOL || e2.t == _BOOL) {
		error_funct(_ERROR_COMPIL, "implicit conversion from type 'boolean' to an other type is not allowed");
		ret = new expression(_ERROR, -1, e1.ref_tab);
	}
	else if (e1.t == _ERROR || e2.t == _ERROR) {
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		ret = new expression(_ERROR, -1, e1.ref_tab);
	}
	else {

		// In this case we can make a huge optimisation
		if (e1.primary_expr && e2.primary_expr) {
			int resi;
			double resd;
			switch (e1.t) {

			case _INT:
				switch (e2.t) {

				case _INT:
					// No conversion needed, both expression have the same type
					if (integer_res != _BOOL && double_res != _BOOL) {
						resi = int_func(e1.int_val, e2.int_val); //compute the operation
						ret = new expression(resi, e1.ref_tab); //store it in a new expression
						ret->primary_expr = true; //the expression is still a primary expr
						ret->int_val = resi; //store it's value
					}
					else {
						resi = int_func(e1.int_val, e2.int_val); //compute the operation
						ret = new expression(integer_res, -1, e1.ref_tab); //store it in a new expression
						ret->primary_expr = true; //the expression is still a primary expr
						ret->int_val = resi; //store it's value
					}
					break;

				case _DOUBLE:
					// Need to convert e1 from int to double
					if (integer_res != _BOOL && double_res != _BOOL) {
						resd = double_func(e1.int_val, e2.double_val); //compute the operation
						ret = new expression(resd, e1.ref_tab); //store it in a new expression
						ret->primary_expr = true; //the expression is still a primary expr
						ret->double_val = resd; //store it's value
					}
					else {
						resi = double_func(e1.int_val, e2.double_val); //compute the operation
						ret = new expression(double_res, -1, e1.ref_tab); //store it in a new expression
						ret->primary_expr = true; //the expression is still a primary expr
						ret->int_val = resi; //store it's value
					}
					break;

				default:
					ret = new expression(_ERROR, -1, e1.ref_tab);
					break;

				}
				break;

			case _DOUBLE:
				switch (e2.t) {

				case _INT:
					// Need to convert e2 from int to double
					if (integer_res != _BOOL && double_res != _BOOL) {
						resd = double_func(e1.double_val, e2.int_val); //compute the operation
						ret = new expression(resd, e1.ref_tab); //store it in a new expression
						ret->primary_expr = true; //the expression is still a primary expr
						ret->double_val = resd; //store it's value
					}
					else {
						resi = double_func(e1.double_val, e2.int_val); //compute the operation
						ret = new expression(double_res, -1, e1.ref_tab); //store it in a new expression
						ret->primary_expr = true; //the expression is still a primary expr
						ret->int_val = resi; //store it's value
					}
					break;

				case _DOUBLE:
					// No conversion needed, both expression have the same type
					if (integer_res != _BOOL && double_res != _BOOL) {
						resd = double_func(e1.double_val, e2.double_val); //compute the operation
						ret = new expression(resd, e1.ref_tab); //store it in a new expression
						ret->primary_expr = true; //the expression is still a primary expr
						ret->double_val = resd; //store it's value
					}
					else {
						resi = double_func(e1.double_val, e2.double_val); //compute the operation
						ret = new expression(double_res, -1, e1.ref_tab); //store it in a new expression
						ret->primary_expr = true; //the expression is still a primary expr
						ret->int_val = resi; //store it's value
					}
					break;

				default:
					ret = new expression(_ERROR, -1, e1.ref_tab);
					break;
				}
				break;

			default:
				ret = new expression(_ERROR, -1, e1.ref_tab);
				break;

			}
		}

		// In this case optimisation is smaller. We directly assign the good value to the result
		if (e1.primary_expr && !e2.primary_expr) {
			char* nb_double;
			int newV;
			switch (e1.t) {

			case _INT:
				switch (e2.t) {

				case _INT:
					// No conversion needed, both expression have the same type
					ret = new expression(integer_res, new_var(), e1.ref_tab);
					ret->code << e2.code.str();
					ret->code << "  %x" << ret->getVar() << " = " << integer_op << " i32 " << e1.int_val << ", %x" << e2.var << "\n";
					break;

				case _DOUBLE:
					// Need to convert e1 from int to double
					nb_double = double_to_hex_str(e1.int_val);
					ret = new expression(double_res, new_var(), e1.ref_tab);
					ret->code << e2.code.str();
					ret->code << "  %x" << ret->getVar() << " = " << double_op << " double " << nb_double << ", %x" << e2.var << "\n";
					free(nb_double);
					break;

				default:
					ret = new expression(_ERROR, -1, e1.ref_tab);
					break;

				}
				break;

			case _DOUBLE:
				switch (e2.t) {

				case _INT:
					// Need to convert e2 from int to double
					newV = new_var();
					nb_double = double_to_hex_str(e1.double_val);
					ret = new expression(double_res, new_var(), e1.ref_tab);
					ret->code << e2.code.str();
					ret->code << "  %x" << newV << " = sitofp i32 %x" << e2.var << " to double\n";
					ret->code << "  %x" << ret->getVar() << " = " << double_op << " double " << nb_double << ", %x" << newV << "\n";
					free(nb_double);
					break;

				case _DOUBLE:
					// No conversion needed, both expression have the same type
					nb_double = double_to_hex_str(e1.double_val);
					ret = new expression(double_res, new_var(), e1.ref_tab);
					ret->code << e2.code.str();
					ret->code << "  %x" << ret->getVar() << " = " << double_op << " double " << nb_double << ", %x" << e2.var << "\n";
					free(nb_double);
					break;

				default:
					ret = new expression(_ERROR, -1, e1.ref_tab);
					break;
				}
				break;

			default:
				ret = new expression(_ERROR, -1, e1.ref_tab);
				break;

			}
		}

		// In this case optimisation is smaller. We directly assign the good value to the result
		if (!e1.primary_expr && e2.primary_expr) {
			char* nb_double;
			int newV;
			switch (e1.t) {

			case _INT:
				switch (e2.t) {

				case _INT:
					// No conversion needed, both expression have the same type
					ret = new expression(integer_res, new_var(), e1.ref_tab);
					ret->code << e1.code.str();
					ret->code << "  %x" << ret->getVar() << " = " << integer_op << " i32 %x" << e1.var << ", " << e2.int_val << "\n";
					break;

				case _DOUBLE:
					// Need to convert e1 from int to double
					newV = new_var();
					nb_double = double_to_hex_str(e2.double_val);
					ret = new expression(double_res, new_var(), e1.ref_tab);
					ret->code << e1.code.str();
					ret->code << "  %x" << newV << " = sitofp i32 %x" << e1.var << " to double\n";
					ret->code << "  %x" << ret->getVar() << " = " << double_op << " double %x" << newV << ", " << nb_double << "\n";
					free(nb_double);
					break;

				default:
					ret = new expression(_ERROR, -1, e1.ref_tab);
					break;

				}
				break;

			case _DOUBLE:
				switch (e2.t) {

				case _INT:
					// Need to convert e2 from int to double
					nb_double = double_to_hex_str(e2.int_val);
					ret = new expression(double_res, new_var(), e1.ref_tab);
					ret->code << e1.code.str();
					ret->code << "  %x" << ret->getVar() << " = " << double_op << " double %x" << e1.var << ", " << nb_double << "\n";
					free(nb_double);
					break;

				case _DOUBLE:
					// No conversion needed, both expression have the same type
					nb_double = double_to_hex_str(e2.double_val);
					ret = new expression(double_res, new_var(), e1.ref_tab);
					ret->code << e1.code.str();
					ret->code << "  %x" << ret->getVar() << " = " << double_op << " double %x" << e1.var << ", " << nb_double << "\n";
					free(nb_double);
					break;

				default:
					ret = new expression(_ERROR, -1, e1.ref_tab);
					break;
				}
				break;

			default:
				ret = new expression(_ERROR, -1, e1.ref_tab);
				break;

			}
		}


	}
	return ret;
}

// Code generation for basics operations between expressions
struct expression* operator+(const struct expression& e1, const struct expression& e2) {
	struct expression* ret = optimize(e1, e2, "add", "fadd", [] (int i1, int i2) {return i1 + i2;}, [] (double i1, double i2) {return i1 + i2;});
	if (ret == NULL)
		return binary_operator(e1, e2, "add", "fadd", _INT, _DOUBLE);
	else
		return ret;
}

struct expression* operator-(const struct expression& e1, const struct expression& e2) {
	struct expression* ret = optimize(e1, e2, "sub", "fsub", [] (int i1, int i2) {return i1 - i2;}, [] (double i1, double i2) {return i1 - i2;});
	if (ret == NULL)
		return binary_operator(e1, e2, "sub", "fsub", _INT, _DOUBLE);
	else
		return ret;
}

struct expression* operator*(const struct expression& e1, const struct expression& e2) {
	struct expression* ret = optimize(e1, e2, "mul", "fmul", [] (int i1, int i2) {return i1 * i2;}, [] (double i1, double i2) {return i1 * i2;});
	if (ret == NULL)
		return binary_operator(e1, e2, "mul", "fmul", _INT, _DOUBLE);
	else
		return ret;
}

struct expression* operator/(const struct expression& e1, const struct expression& e2) {
	struct expression* ret = optimize(e1, e2, "sdiv", "fdiv", [] (int i1, int i2) {return i1 / i2;}, [] (double i1, double i2) {return i1 / i2;});
	if (ret == NULL)
		return binary_operator(e1, e2, "sdiv", "fdiv", _INT, _DOUBLE);
	else
		return ret;
}

struct expression* operator%(const struct expression& e1, const struct expression& e2) {
	struct expression* ret = optimize(e1, e2, "srem", "frem", [] (int i1, int i2) {return i1 % i2;}, [] (double i1, double i2) {return fmod(i1, i2);});
	if (ret == NULL)
		return binary_operator(e1, e2, "srem", "frem", _INT, _DOUBLE);
	else
		return ret;
}


struct expression* operator<<(const struct expression& e1, const struct expression& e2) {
	//This operation is only permitted if e1 and e2 are integers.
	struct expression* ret;

	// Error gestion : if used in order to avoid repetition in the switch
	if (e1.t == _VOID || e2.t == _VOID) {
		error_funct(_ERROR_COMPIL, "invalid use of void expression");
		ret = new expression(_ERROR, -1, e1.ref_tab);
	}
	else if (e1.t == _BOOL || e2.t == _BOOL) {
		error_funct(_ERROR_COMPIL, "implicit conversion from type 'boolean' to an other type is not allowed");
		ret = new expression(_ERROR, -1, e1.ref_tab);
	}
	else if (e1.t == _ERROR || e2.t == _ERROR) {
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		ret = new expression(_ERROR, -1, e1.ref_tab);
	}
	else {
		int resi;
		switch (e1.t) {
		case _INT:

			switch (e2.t) {

			case _INT:
				//in this case we can diretcly compute the value
				cerr << e1.primary_expr << e2.primary_expr << endl;
				if (e1.primary_expr && e2.primary_expr){
					resi = e1.int_val << e2.int_val;
					ret = new expression(_INT, -1, e1.ref_tab); //store it in a new expression
					ret->primary_expr = true; //the expression is still a primary expr
					ret->int_val = resi; //store it's value
				}
				else {
					ret = new expression(_INT, new_var(), e1.ref_tab);
					if (!e1.primary_expr) ret->code << e1.code.str();
					if (!e2.primary_expr) ret->code << e2.code.str();
					ret->code << "  %x" << ret->getVar() << " = shl i32 ";
					if (e1.primary_expr)
						ret->code << e1.int_val;
					else
						ret->code << "%x" << e1.var;
					ret->code << ", ";
					if (e2.primary_expr)
						ret->code << e2.int_val;
					else
						ret->code << "%x" << e2.var;
					ret->code << "\n";
				}
				break;

			case _DOUBLE:
				error_funct(_ERROR_COMPIL, "SHL operation canno't be applied to a double expression");
				ret = new expression(_ERROR, -1, e1.ref_tab);
				break;

			default:
				ret = new expression(_ERROR, -1, e1.ref_tab);
				break;
			}

			break;

		case _DOUBLE:
			error_funct(_ERROR_COMPIL, "SHL operation canno't be applied to a double expression");
			ret = new expression(_ERROR, -1, e1.ref_tab);
			break;

		default:
			ret = new expression(_ERROR, -1, e1.ref_tab);
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
		ret = new expression(_ERROR, -1, e1.ref_tab);
	}
	else if (e1.t == _BOOL || e2.t == _BOOL) {
		error_funct(_ERROR_COMPIL, "implicit conversion from type 'boolean' to an other type is not allowed");
		ret = new expression(_ERROR, -1, e1.ref_tab);
	}
	else if (e1.t == _ERROR || e2.t == _ERROR) {
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		ret = new expression(_ERROR, -1, e1.ref_tab);
	}
	else {
		int resi;
		switch (e1.t) {
		case _INT:

			switch (e2.t) {

			case _INT:
				//in this case we can diretcly compute the value
				if (e1.primary_expr && e2.primary_expr){
					resi = e1.int_val >> e2.int_val;
					ret = new expression(_INT, -1, e1.ref_tab); //store it in a new expression
					ret->primary_expr = true; //the expression is still a primary expr
					ret->int_val = resi; //store it's value
				}
				else {
					ret = new expression(_INT, new_var(), e1.ref_tab);
					if (!e1.primary_expr) ret->code << e1.code.str();
					if (!e2.primary_expr) ret->code << e2.code.str();
					ret->code << "  %x" << ret->getVar() << " = ashr i32 ";
					if (e1.primary_expr)
						ret->code << e1.int_val;
					else
						ret->code << "%x" << e1.var;
					ret->code << ", ";
					if (e2.primary_expr)
						ret->code << e2.int_val;
					else
						ret->code << "%x" << e2.var;
					ret->code << "\n";
				}
				break;

			case _DOUBLE:
				error_funct(_ERROR_COMPIL, "SHL operation canno't be applied to a double expression");
				ret = new expression(_ERROR, -1, e1.ref_tab);

				break;

			default:
				ret = new expression(_ERROR, -1, e1.ref_tab);
				break;
			}

			break;

		case _DOUBLE:
			error_funct(_ERROR_COMPIL, "SHR operation canno't be applied to a double expression");
			ret = new expression(_ERROR, -1, e1.ref_tab);
			break;

		default:
			ret = new expression(_ERROR, -1, e1.ref_tab);
			break;
		}
	}
	return ret;
}


// Code generation for assignements
struct expression* expression::operator=(string s) {
	struct expression* ret;

	list<map_boost>::iterator it;
	bool error = true;
	for(it=ref_tab.begin(); it != ref_tab.end(); ++it) {
		if ((*it).find(s) != (*it).end()) {
			error = false;
			goto end;
		}
	}
end:

	if (error) {
		ret = new expression(_ERROR, -1, ref_tab);
		error_funct(_ERROR_COMPIL, s, " was not declared in this scope");
	}
	else {
		map_boost& hash = *it;
		struct identifier &id = hash.at(s);
		id.used = true;
		if (id.symbolType != _LOCAL_VAR && id.symbolType != _GLOBAL_VAR) {
			// In this case, we try to assign an expression to an identifier which doesn't accept one
			// For exemple we try to assign an expression to a function name
			ret = new expression(_ERROR, -1, ref_tab);
			error_funct(_ERROR_COMPIL, "Can't assign an expression to ", s);
		}
		else {
			int newVar = var;

			// Error gestion : if used in order to avoid repetition in the switch
			if (t == _VOID || id.t == _VOID) {
				error_funct(_ERROR_COMPIL, "invalid use of void expression");
				ret = new expression(_ERROR, -1, ref_tab);
			}
			else if (t == _BOOL || id.t == _BOOL) {
				error_funct(_ERROR_COMPIL, "implicit conversion from type 'boolean' to an other type is not allowed");
				ret = new expression(_ERROR, -1, ref_tab);
			}
			else if (t == _ERROR || id.t == _ERROR) {
				// In this case, we consider that the expression has already an error and we don't consider others errors
				// in order to don't flood the error output with big expressions.
				ret = new expression(_ERROR, -1, ref_tab);
			}
			else {

				// Optimisation if the expression has a simple value
				if (primary_expr) {

					char* nb_double;
					switch (id.t) {

					case _INT:
						switch (t) {

						case _INT:
							// No conversion needed
							ret = new expression(_INT, var, ref_tab);
							ret->code << "  store i32 " << int_val << ", i32* " << id.name << "\n";
							break;

						case _DOUBLE:
							// Need to convert expression from double to int
							ret = new expression(_INT, var, ref_tab);
							ret->code << "  store i32 " << (int) double_val << ", i32* " << id.name << "\n";
							break;

						default:
							ret = new expression(_ERROR, -1, ref_tab);
							break;

						}
						break;

					case _DOUBLE:
						switch (t) {

						case _INT:
							// Need to convert expression from int to double
							nb_double = double_to_hex_str(int_val);
							ret = new expression(_DOUBLE, var, ref_tab);
							ret->code << "  store double " << nb_double << ", double* " << id.name << "\n";
							free(nb_double);
							break;

						case _DOUBLE:
							// No conversion needed
							nb_double = double_to_hex_str(double_val);
							ret = new expression(_DOUBLE, var, ref_tab);
							ret->code << "  store double " << nb_double << ", double* " << id.name << "\n";
							free(nb_double);
							break;

						default:
							ret = new expression(_ERROR, -1, ref_tab);
							break;

						}
						break;

					default:
						ret = new expression(_ERROR, -1, ref_tab);
						break;
					}



				}
				else {
					switch (id.t) {

					case _INT:
						switch (t) {

						case _INT:
							// No conversion needed
							ret = new expression(_INT, var, ref_tab);
							ret->code << code.str();
							ret->code << "  store i32 %x" << var << ", i32* " << id.name << "\n";
							break;

						case _DOUBLE:
							// Need to convert expression from double to int
							newVar = new_var();
							ret = new expression(_INT, newVar, ref_tab);
							ret->code << code.str();
							ret->code << "  %x" << newVar <<  " = fptosi double %x" << var << " to i32\n";
							ret->code << "  store i32 %x" << newVar << ", i32* " << id.name << "\n";
							break;

						default:
							ret = new expression(_ERROR, -1, ref_tab);
							break;

						}
						break;

					case _DOUBLE:
						switch (t) {
						case _INT:
							// Need to convert expression from int to double
							newVar = new_var();
							ret = new expression(_DOUBLE, newVar, ref_tab);
							ret->code << code.str();
							ret->code << "  %x" << newVar << " = sitofp i32 %x" << var << " to double\n";
							ret->code << "  store double %x" << newVar << ", double* " << id.name << "\n";
							break;

						case _DOUBLE:
							// No conversion needed
							ret = new expression(_DOUBLE, var, ref_tab);
							ret->code << code.str();
							ret->code << "  store double %x" << var << ", double* " << id.name << "\n";
							break;

						default:
							ret = new expression(_ERROR, -1, ref_tab);
							break;

						}
						break;

					default:
						ret = new expression(_ERROR, -1, ref_tab);
						break;
					}
				}
			}
		}
	}
	return ret;
}

struct expression* expression::operator+=(string s) {
	struct expression* ret;
	struct expression* e1 = new expression(s, ref_tab);
	struct expression* e2 = *e1 + *this;
	ret = (*e2 = s);
	delete e1; e1 = nullptr; delete e2; e2 = nullptr;
	return ret;
}

struct expression* expression::operator-=(string s) {
	struct expression* ret;
	struct expression* e1 = new expression(s, ref_tab);
	struct expression* e2 = *e1 - *this;
	ret = (*e2 = s);
	delete e1; e1 = nullptr; delete e2; e2 = nullptr;
	return ret;
}

struct expression* expression::operator*=(string s) {
	struct expression* ret;
	struct expression* e1 = new expression(s, ref_tab);
	struct expression* e2 = *e1** this;
	ret = (*e2 = s);
	delete e1; e1 = nullptr; delete e2; e2 = nullptr;
	return ret;
}

struct expression* expression::operator/=(string s) {
	struct expression* ret;
	struct expression* e1 = new expression(s, ref_tab);
	struct expression* e2 = *e1 / *this;
	ret = (*e2 = s);
	delete e1; e1 = nullptr; delete e2; e2 = nullptr;
	return ret;
}

struct expression* expression::operator%=(string s) {
	struct expression* ret;
	struct expression* e1 = new expression(s, ref_tab);
	struct expression* e2 = *e1 % *this;
	ret = (*e2 = s);
	delete e1; e1 = nullptr; delete e2; e2 = nullptr;
	return ret;
}

struct expression* expression::operator<<=(string s) {
	struct expression* ret;
	struct expression* e1 = new expression(s, ref_tab);
	struct expression* e2 = *e1 << *this;
	ret = (*e2 = s);
	delete e1; e1 = nullptr; delete e2; e2 = nullptr;
	return ret;
}

struct expression* expression::operator>>=(string s) {
	struct expression* ret;
	struct expression* e1 = new expression(s, ref_tab);
	struct expression* e2 = *e1 >> *this;
	ret = (*e2 = s);
	delete e1; e1 = nullptr; delete e2; e2 = nullptr;
	return ret;
}


// Code generation for conditionals expression
struct expression* operator==(const struct expression& e1, const struct expression& e2) {
	struct expression* ret = optimize(e1, e2, "icmp eq", "fcmp oeq",
									  [] (int i1, int i2) {if (i1 == i2) return 1; else return 0;},
	[] (double i1, double i2) {if (i1 == i2) return 1; else return 0;},
	_BOOL, _BOOL);
	if (ret == NULL)
		return binary_operator(e1, e2, "icmp eq", "fcmp oeq", _BOOL, _BOOL);
	else
		return ret;
}

struct expression* operator!=(const struct expression& e1, const struct expression& e2) {
	struct expression* ret = optimize(e1, e2, "icmp ne", "fcmp one",
									  [] (int i1, int i2) {if (i1 != i2) return 1; else return 0;},
	[] (double i1, double i2) {if (i1 != i2) return 1; else return 0;},
	_BOOL, _BOOL);
	if (ret == NULL)
		return binary_operator(e1, e2, "icmp ne", "fcmp one", _BOOL, _BOOL);
	else
		return ret;
}

struct expression* operator<(const struct expression& e1, const struct expression& e2) {
	struct expression* ret = optimize(e1, e2, "icmp slt", "fcmp olt",
									  [] (int i1, int i2) {if (i1 < i2) return 1; else return 0;},
	[] (double i1, double i2) {if (i1 < i2) return 1; else return 0;},
	_BOOL, _BOOL);
	if (ret == NULL)
		return binary_operator(e1, e2, "icmp slt", "fcmp olt", _BOOL, _BOOL);
	else
		return ret;
}

struct expression* operator>(const struct expression& e1, const struct expression& e2) {
	struct expression* ret = optimize(e1, e2, "icmp sgt", "fcmp ogt",
									  [] (int i1, int i2) {if (i1 > i2) return 1; else return 0;},
	[] (double i1, double i2) {if (i1 > i2) return 1; else return 0;},
	_BOOL, _BOOL);
	if (ret == NULL)
		return binary_operator(e1, e2, "icmp sgt", "fcmp ogt", _BOOL, _BOOL);
	else
		return ret;
}

struct expression* operator<=(const struct expression& e1, const struct expression& e2) {
	struct expression* ret = optimize(e1, e2, "icmp sle", "fcmp ole",
									  [] (int i1, int i2) {if (i1 <= i2) return 1; else return 0;},
	[] (double i1, double i2) {if (i1 <= i2) return 1; else return 0;},
	_BOOL, _BOOL);
	if (ret == NULL)
		return binary_operator(e1, e2, "icmp sle", "fcmp ole", _BOOL, _BOOL);
	else
		return ret;
}

struct expression* operator>=(const struct expression& e1, const struct expression& e2) {
	struct expression* ret = optimize(e1, e2, "icmp sge", "fcmp oge",
									  [] (int i1, int i2) {if (i1 >= i2) return 1; else return 0;},
	[] (double i1, double i2) {if (i1 >= i2) return 1; else return 0;},
	_BOOL, _BOOL);
	if (ret == NULL)
		return binary_operator(e1, e2, "icmp sge", "fcmp oge", _BOOL, _BOOL);
	else
		return ret;
}


// Code generation for logicales operators
struct expression* operator&&(const struct expression& e1, const struct expression& e2) {
	struct expression* ret;

	// Error gestion : if used in order to avoid repetition in the switch
	if (e1.t == _VOID || e2.t == _VOID) {
		error_funct(_ERROR_COMPIL, "invalid use of void expression");
		ret = new expression(_ERROR, -1, e1.ref_tab);
	}
	else if (e1.t == _INT || e1.t == _DOUBLE || e2.t == _INT || e2.t == _DOUBLE) {
		error_funct(_ERROR_COMPIL, "Can't apply a logical operator between booleans and other types");
		ret = new expression(_ERROR, -1, e1.ref_tab);
	}
	else if (e1.t == _ERROR || e2.t == _ERROR) {
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		ret = new expression(_ERROR, -1, e1.ref_tab);
	}
	else {
		int resi;
		switch (e1.t) {
		case _BOOL:

			switch (e2.t) {

			case _BOOL:

				//in this case we can diretcly compute the value
				if (e1.primary_expr && e2.primary_expr){
					if (e1.int_val == 1 && e2.int_val == 1) resi = 1;
					else resi = 0;
					ret = new expression(_BOOL, -1, e1.ref_tab); //store it in a new expression
					ret->primary_expr = true; //the expression is still a primary expr
					ret->int_val = resi; //store it's value
				}
				else {
					ret = new expression(_BOOL, new_var(), e1.ref_tab);
					if (!e1.primary_expr) ret->code << e1.code.str();
					if (!e2.primary_expr) ret->code << e2.code.str();
					ret->code << "  %x" << ret->getVar() << " = and i1 ";
					if (e1.primary_expr)
						ret->code << e1.int_val;
					else
						ret->code << "%x" << e1.var;
					ret->code << ", ";
					if (e2.primary_expr)
						ret->code << e2.int_val;
					else
						ret->code << "%x" << e2.var;
					ret->code << "\n";
				}
				break;

			default:
				ret = new expression(_ERROR, -1, e1.ref_tab);
				break;
			}

			break;

		default:
			ret = new expression(_ERROR, -1, e1.ref_tab);
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
		ret = new expression(_ERROR, -1, e1.ref_tab);
	}
	else if (e1.t == _INT || e1.t == _DOUBLE || e2.t == _INT || e2.t == _DOUBLE) {
		error_funct(_ERROR_COMPIL, "Can't apply a logical operator between booleans and other types");
		ret = new expression(_ERROR, -1, e1.ref_tab);
	}
	else if (e1.t == _ERROR || e2.t == _ERROR) {
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		ret = new expression(_ERROR, -1, e1.ref_tab);
	}
	else {
		int resi;
		switch (e1.t) {
		case _BOOL:

			switch (e2.t) {

			case _BOOL:
				//in this case we can diretcly compute the value
				if (e1.primary_expr && e2.primary_expr){
					if (e1.int_val == 1 || e2.int_val == 1) resi = 1;
					else resi = 0;
					ret = new expression(_BOOL, -1, e1.ref_tab); //store it in a new expression
					ret->primary_expr = true; //the expression is still a primary expr
					ret->int_val = resi; //store it's value
				}
				else {
					ret = new expression(_BOOL, new_var(), e1.ref_tab);
					if (!e1.primary_expr) ret->code << e1.code.str();
					if (!e2.primary_expr) ret->code << e2.code.str();
					ret->code << "  %x" << ret->getVar() << " = or i1 ";
					if (e1.primary_expr)
						ret->code << e1.int_val;
					else
						ret->code << "%x" << e1.var;
					ret->code << ", ";
					if (e2.primary_expr)
						ret->code << e2.int_val;
					else
						ret->code << "%x" << e2.var;
					ret->code << "\n";
				}
				break;

			default:
				ret = new expression(_ERROR, -1, e1.ref_tab);
				break;
			}

			break;

		default:
			ret = new expression(_ERROR, -1, e1.ref_tab);
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
		ret = new expression(_ERROR, -1, e1.ref_tab);
	}
	else if (e1.t == _INT || e1.t == _DOUBLE) {
		error_funct(_ERROR_COMPIL, "Can't apply a logical operator between booleans and other types");
		ret = new expression(_ERROR, -1, e1.ref_tab);
	}
	else if (e1.t == _ERROR) {
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		ret = new expression(_ERROR, -1, e1.ref_tab);
	}
	else {
		int resi;
		switch (e1.t) {
		case _BOOL:
			//in this case we can diretcly compute the value
			if (e1.primary_expr){
				if (e1.int_val == 1) resi = 0;
				else resi = 1;
				ret = new expression(_BOOL, -1, e1.ref_tab); //store it in a new expression
				ret->primary_expr = true; //the expression is still a primary expr
				ret->int_val = resi; //store it's value
			}
			else {
				ret = new expression(_BOOL, new_var(), e1.ref_tab);
				ret->code << e1.code.str();
				ret->code << "  %x" << ret->getVar() << " = xor i1 %x" << e1.var << ", 1\n";
			}
			break;

		default:
			ret = new expression(_ERROR, -1, e1.ref_tab);
			break;
		}
	}
	return ret;
}


// unary operators expressions
struct expression *incr_postfix(string name, map_list &ref_table) {
	struct expression *ret;
	struct expression e1(name, ref_table);

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
		ret = new expression(_ERROR, -1, ref_table);
		break;

	case _BOOL:
		error_funct(_ERROR_COMPIL, "implicit conversion from type 'boolean' to an other type is not allowed");
		ret = new expression(_ERROR, -1, ref_table);
		break;

	case _ERROR:
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		ret = new expression(_ERROR, -1, ref_table);
		break;

	default:
		// If you see this something really goes wrong withe the compiler
		error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
		ret = new expression(_ERROR, -1, ref_table);
		break;
	}
	return ret;
}

struct expression *decr_postfix(string name, map_list &ref_table) {
	struct expression *ret;
	struct expression e1(name, ref_table);

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
		ret = new expression(_ERROR, -1, ref_table);
		break;

	case _BOOL:
		error_funct(_ERROR_COMPIL, "implicit conversion from type 'boolean' to an other type is not allowed");
		ret = new expression(_ERROR, -1, ref_table);
		break;

	case _ERROR:
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		ret = new expression(_ERROR, -1, ref_table);
		break;

	default:
		// If you see this something really goes wrong withe the compiler
		error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
		ret = new expression(_ERROR, -1, ref_table);
		break;
	}
	return ret;
}

struct expression *incr_prefix(string name, map_list &ref_tab) {
	struct expression *ret;
	struct expression e1(name, ref_tab);

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
		ret = new expression(_ERROR, -1, ref_tab);
		break;

	case _BOOL:
		error_funct(_ERROR_COMPIL, "implicit conversion from type 'boolean' to an other type is not allowed");
		ret = new expression(_ERROR, -1, ref_tab);
		break;

	case _ERROR:
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		ret = new expression(_ERROR, -1, ref_tab);
		break;

	default:
		// If you see this something really goes wrong withe the compiler
		error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
		ret = new expression(_ERROR, -1, ref_tab);
		break;
	}
	return ret;
}

struct expression *decr_prefix(string name, map_list &ref_tab) {
	struct expression *ret;
	struct expression e1(name, ref_tab);

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
		ret = new expression(_ERROR, -1, ref_tab);
		break;

	case _BOOL:
		error_funct(_ERROR_COMPIL, "implicit conversion from type 'boolean' to an other type is not allowed");
		ret = new expression(_ERROR, -1, ref_tab);
		break;

	case _ERROR:
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		ret = new expression(_ERROR, -1, ref_tab);
		break;

	default:
		// If you see this something really goes wrong withe the compiler
		error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
		ret = new expression(_ERROR, -1, ref_tab);
		break;
	}
	return ret;
}

struct expression *opposite(const struct expression &e1){
	struct expression *ret;



	// we can optimize if e1 has a direct value
	if (e1.primary_expr) {
		int resi;
		double resd;
		switch (e1.getT()) {
		case _INT:
			resi = - e1.int_val;
			ret = new expression(resi, e1.ref_tab); //store result
			ret->primary_expr = true; //the expression is still a primary expr
			ret->int_val = resi; //store it's value
			break;

		case _DOUBLE:
			resd = - e1.double_val;
			ret = new expression(resd, e1.ref_tab); //store result
			ret->primary_expr = true; //the expression is still a primary expr
			ret->int_val = resd; //store it's value
			break;

		case _VOID:
			error_funct(_ERROR_COMPIL, "invalid use of void expression");
			ret = new expression(_ERROR, -1, e1.ref_tab);
			break;

		case _BOOL:
			error_funct(_ERROR_COMPIL, "implicit conversion from type 'boolean' to an other type is not allowed");
			ret = new expression(_ERROR, -1, e1.ref_tab);
			break;

		case _ERROR:
			// In this case, we consider that the expression has already an error and we don't consider others errors
			// in order to don't flood the error output with big expressions.
			ret = new expression(_ERROR, -1, e1.ref_tab);
			break;

		default:
			// If you see this something really goes wrong withe the compiler
			error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
			ret = new expression(_ERROR, -1, e1.ref_tab);
			break;
		}
	}
	else {
		switch (e1.getT()) {
		case _INT:
			ret = new expression(_INT, new_var(), e1.ref_tab);
			ret->code << e1.code.str();
			ret->code << "  %x" << ret->getVar() << " = sub i32 0, %x" << e1.getVar() << "\n";
			break;

		case _DOUBLE:
			ret = new expression(_DOUBLE, new_var(), e1.ref_tab);
			ret->code << e1.code.str();
			ret->code << "  %x" << ret->getVar() << " = fsub double 0x000000000000000, %x" << e1.getVar() << "\n";
			break;

		case _VOID:
			error_funct(_ERROR_COMPIL, "invalid use of void expression");
			ret = new expression(_ERROR, -1, e1.ref_tab);
			break;

		case _BOOL:
			error_funct(_ERROR_COMPIL, "implicit conversion from type 'boolean' to an other type is not allowed");
			ret = new expression(_ERROR, -1, e1.ref_tab);
			break;

		case _ERROR:
			// In this case, we consider that the expression has already an error and we don't consider others errors
			// in order to don't flood the error output with big expressions.
			ret = new expression(_ERROR, -1, e1.ref_tab);
			break;

		default:
			// If you see this something really goes wrong withe the compiler
			error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
			ret = new expression(_ERROR, -1, e1.ref_tab);
			break;
		}
	}
	return ret;
}
