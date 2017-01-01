#include "statement.hpp"

struct code_container* if_then_else(const struct expression& e1,
									const struct code_container& s1) {
	struct code_container* ret;
	//IF is posible only if expression is a boolean
	int l1, l2;

	switch (e1.getT()) {

	case _BOOL:
		l1 = new_label();
		l2 = new_label();
		ret = new code_container();
		ret->code << e1.code.str(); //Compute the expression
		ret->code << "  br i1 %x" << e1.getVar() << ", label %label" << l1 << ", label %label" << l2 << "\n"; //jump command
		ret->code << "\nlabel" << l1 << ": ; if.then\n" << s1.code.str() << "  br label %label" << l2 << "\n" ; //then command
		ret->code << "\nlabel" << l2 << ": ; if.end\n"; //ifend command
		break;

	case _INT:
		error_funct(_ERROR_COMPIL, "Can't use integer expression in an if statement");
		break;

	case _DOUBLE:
		error_funct(_ERROR_COMPIL, "Can't use double expression in an if statement");
		break;

	case _VOID:
		error_funct(_ERROR_COMPIL, "Can't use void expression in an if statement");
		break;

	case _ERROR:
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		break;

	default:
		// If you see this something really goes wrong withe the compiler
		error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
		break;

	}

	return ret;
}

struct code_container* if_then_else(const struct expression& e1,
									const struct code_container& s1, const struct code_container& s2) {
	struct code_container* ret;
	//IF is posible only if expression is a boolean
	int l1, l2, l3;

	switch (e1.getT()) {
	case _BOOL:
		l1 = new_label();
		l2 = new_label();
		l3 = new_label();
		ret = new code_container();
		ret->code << e1.code.str(); //Compute the expression
		ret->code << "  br i1 %x" << e1.getVar() << ", label %label" << l1 << ", label %label" << l2 << "\n"; //jump command
		ret->code << "\nlabel" << l1 << ": ; if.then\n" << s1.code.str() << "  br label %label" << l3 << "\n" ; //then command
		ret->code << "\nlabel" << l2 << ": ; if.else\n" << s2.code.str() << "  br label %label" << l3 << "\n" ; //else command
		ret->code << "\nlabel" << l3 << ": ; if.end\n"; //ifend command
		break;

	case _INT:
		error_funct(_ERROR_COMPIL, "Can't use integer expression in an if statement");
		break;

	case _DOUBLE:
		error_funct(_ERROR_COMPIL, "Can't use double expression in an if statement");
		break;

	case _VOID:
		error_funct(_ERROR_COMPIL, "Can't use void expression in an if statement");
		break;

	case _ERROR:
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		break;

	default:
		// If you see this something really goes wrong withe the compiler
		error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
		break;

	}

	return ret;
}


struct code_container* for_then(const struct expression& e1, const struct expression& e2,
								const struct expression& e3, const struct code_container& s1) {
	struct code_container* ret;
	//FOR is posible only if e2 is a boolean
	int l1, l2, l3, l4;

	switch (e2.getT()) {

	case _BOOL:
		l1 = new_label();
		l2 = new_label();
		l3 = new_label();
		l4 = new_label();
		ret = new code_container();
		ret->code << e1.code.str() << "  br label %label" << l1 << "\n"; //init expression and go to beggining
		ret->code << "\nlabel" << l1 << ": ; for.cond\n" << e2.code.str(); //for cond evaluation
		ret->code << "  br i1 %x" << e2.getVar() << ", label %label" << l2 << ", label %label" << l4 << "\n"; //jump command
		ret->code << "\nlabel" << l2 << ": ; for.body\n" << s1.code.str(); //for body evaluation
		ret->code << "  br label %label" << l3 << "\n" ; //go to inc
		ret->code << "\nlabel" << l3 << ": ; for.inc\n" << e3.code.str(); //for inc evaluation
		ret->code << "  br label %label" << l1 << "\n" ; //go to cond
		ret->code << "\nlabel" << l4 << ": ; for.end\n"; //end label
		break;

	case _INT:
		error_funct(_ERROR_COMPIL, "Can't use integer expression in an for statement");
		break;

	case _DOUBLE:
		error_funct(_ERROR_COMPIL, "Can't use double expression in an for statement");
		break;

	case _VOID:
		error_funct(_ERROR_COMPIL, "Can't use void expression in an for statement");
		break;

	case _ERROR:
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		break;

	default:
		// If you see this something really goes wrong withe the compiler
		error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
		break;

	}

	return ret;
}

struct code_container* for_then(const struct expression& e1, const struct expression& e2,
								void*, const struct code_container& s1) {
	struct code_container* ret;
	//FOR is posible only if e2 is a boolean
	int l1, l2, l3, l4;

	switch (e2.getT()) {

	case _BOOL:
		l1 = new_label();
		l2 = new_label();
		l3 = new_label();
		l4 = new_label();
		ret = new code_container();
		ret->code << e1.code.str() << "  br label %label" << l1 << "\n"; //init expression and go to beggining
		ret->code << "\nlabel" << l1 << ": ; for.cond\n" << e2.code.str(); //for cond evaluation
		ret->code << "  br i1 %x" << e2.getVar() << ", label %label" << l2 << ", label %label" << l4 << "\n"; //jump command
		ret->code << "\nlabel" << l2 << ": ; for.body\n" << s1.code.str(); //for body evaluation
		ret->code << "  br label %label" << l3 << "\n" ; //go to inc
		ret->code << "\nlabel" << l3 << ": ; for.inc\n"; //for inc evaluation
		ret->code << "  br label %label" << l1 << "\n" ; //go to cond
		ret->code << "\nlabel" << l4 << ": ; for.end\n"; //end label
		break;

	case _INT:
		error_funct(_ERROR_COMPIL, "Can't use integer expression in an for statement");
		break;

	case _DOUBLE:
		error_funct(_ERROR_COMPIL, "Can't use double expression in an for statement");
		break;

	case _VOID:
		error_funct(_ERROR_COMPIL, "Can't use void expression in an for statement");
		break;

	case _ERROR:
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		break;

	default:
		// If you see this something really goes wrong withe the compiler
		error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
		break;

	}

	return ret;
}

struct code_container* for_then(const struct expression& e1, void*,
								const struct expression& e3, const struct code_container& s1) {
	struct code_container* ret;
	// No need to comparaison because no test, always go to beggining
	int l1, l2, l3, l4;
	l1 = new_label();
	l2 = new_label();
	l3 = new_label();
	l4 = new_label();
	ret = new code_container();
	ret->code << e1.code.str() << "  br label %label" << l1 << "\n"; //init expression and go to beggining
	ret->code << "\nlabel" << l1 << ": ; for.cond\n"; //for cond evaluation
	ret->code << "  br label %label" << l2 << "\n"; //direct jump command
	ret->code << "\nlabel" << l2 << ": ; for.body\n" << s1.code.str(); //for body evaluation
	ret->code << "  br label %label" << l3 << "\n" ; //go to inc
	ret->code << "\nlabel" << l3 << ": ; for.inc\n" << e3.code.str(); //for inc evaluation
	ret->code << "  br label %label" << l1 << "\n" ; //go to cond
	ret->code << "\nlabel" << l4 << ": ; for.end\n"; //end label
	return ret;
}

struct code_container* for_then(const struct expression& e1, void*, void*,
								const struct code_container& s1) {
	struct code_container* ret;
	// No need to comparaison because no test, always go to beggining
	int l1, l2, l3, l4;
	l1 = new_label();
	l2 = new_label();
	l3 = new_label();
	l4 = new_label();
	ret = new code_container();
	ret->code << e1.code.str() << "  br label %label" << l1 << "\n"; //init expression and go to beggining
	ret->code << "\nlabel" << l1 << ": ; for.cond\n"; //for cond evaluation
	ret->code << "  br label %label" << l2 << "\n"; //direct jump command
	ret->code << "\nlabel" << l2 << ": ; for.body \n" << s1.code.str(); //for body evaluation
	ret->code << "  br label %label" << l3 << "\n" ; //go to inc
	ret->code << "\nlabel" << l3 << ": ; for.inc\n"; //for inc evaluation
	ret->code << "  br label %label" << l1 << "\n" ; //go to cond
	ret->code << "\nlabel" << l4 << ": ; for.end\n"; //end label
	return ret;
}

struct code_container* for_then(void*, const struct expression& e2,
								const struct expression& e3, const struct code_container& s1) {
	struct code_container* ret;
	//FOR is posible only if e2 is a boolean
	int l1, l2, l3, l4;

	switch (e2.getT()) {
	case _BOOL:
		l1 = new_label();
		l2 = new_label();
		l3 = new_label();
		l4 = new_label();
		ret = new code_container();
		ret->code << "  br label %label" << l1 << "\n" ; //init expression and go to beggining
		ret->code << "\nlabel" << l1 << ": ; for.cond\n" << e2.code.str(); //for cond evaluation
		ret->code << "  br i1 %x" << e2.getVar() << ", label %label" << l2 << ", label %label" << l4 << "\n"; //jump command
		ret->code << "\nlabel" << l2 << ": ; for.body\n" << s1.code.str(); //for body evaluation
		ret->code << "  br label %label" << l3 << "\n" ; //go to inc
		ret->code << "\nlabel" << l3 << ": ; for.inc\n" << e3.code.str(); //for inc evaluation
		ret->code << "  br label %label" << l1 << "\n" ; //go to cond
		ret->code << "\nlabel" << l4 << ": ; for.end\n"; //end label
		break;

	case _INT:
		error_funct(_ERROR_COMPIL, "Can't use integer expression in an for statement");
		break;

	case _DOUBLE:
		error_funct(_ERROR_COMPIL, "Can't use double expression in an for statement");
		break;

	case _VOID:
		error_funct(_ERROR_COMPIL, "Can't use void expression in an for statement");
		break;

	case _ERROR:
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		break;

	default:
		// If you see this something really goes wrong withe the compiler
		error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
		break;

	}

	return ret;
}

struct code_container* for_then(void*, const struct expression& e2, void*,
								const struct code_container& s1) {
	struct code_container* ret;
	//FOR is posible only if e2 is a boolean
	int l1, l2, l3, l4;

	switch (e2.getT()) {
	case _BOOL:
		l1 = new_label();
		l2 = new_label();
		l3 = new_label();
		l4 = new_label();
		ret = new code_container();
		ret->code << "  br label %label" << l1 << "\n" ; //init expression and go to beggining
		ret->code << "\nlabel" << l1 << ": ; for.cond\n" << e2.code.str(); //for cond evaluation
		ret->code << "  br i1 %x" << e2.getVar() << ", label %label" << l2 << ", label %label" << l4 << "\n"; //jump command
		ret->code << "\nlabel" << l2 << ": ; for.body\n" << s1.code.str(); //for body evaluation
		ret->code << "  br label %label" << l3 << "\n" ; //go to inc
		ret->code << "\nlabel" << l3 << ": ; for.inc\n"; //for inc evaluation
		ret->code << "  br label %label" << l1 << "\n" ; //go to cond
		ret->code << "\nlabel" << l4 << ": ; for.end\n"; //end label
		break;

	case _INT:
		error_funct(_ERROR_COMPIL, "Can't use integer expression in an for statement");
		break;

	case _DOUBLE:
		error_funct(_ERROR_COMPIL, "Can't use double expression in an for statement");
		break;

	case _VOID:
		error_funct(_ERROR_COMPIL, "Can't use void expression in an for statement");
		break;

	case _ERROR:
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		break;

	default:
		// If you see this something really goes wrong withe the compiler
		error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
		break;

	}

	return ret;
}

struct code_container* for_then(void*, void*, const struct expression& e3,
								const struct code_container& s1) {
	struct code_container* ret;
	// No need to comparaison because no test, always go to beggining
	int l1, l2, l3, l4;
	l1 = new_label();
	l2 = new_label();
	l3 = new_label();
	l4 = new_label();
	ret = new code_container();
	ret->code << "  br label %label" << l1 << "\n" ; //init expression and go to beggining
	ret->code << "\nlabel" << l1 << ": ; for.cond\n"; //for cond evaluation
	ret->code << "  br label %label" << l2 << "\n"; //direct jump command
	ret->code << "\nlabel" << l2 << ": ; for.body\n" << s1.code.str(); //for body evaluation
	ret->code << "  br label %label" << l3 << "\n" ; //go to inc
	ret->code << "\nlabel" << l3 << ": ; for.inc\n" << e3.code.str(); //for inc evaluation
	ret->code << "  br label %label" << l1 << "\n" ; //go to cond
	ret->code << "\nlabel" << l4 << ": ; for.end\n"; //end label
	return ret;
}

struct code_container* for_then(void*, void*, void*, const struct code_container& s1) {
	struct code_container* ret;
	// No need to comparaison because no test, always go to beggining
	int l1, l2, l3, l4;
	l1 = new_label();
	l2 = new_label();
	l3 = new_label();
	l4 = new_label();
	ret = new code_container();
	ret->code << "  br label %label" << l1 << "\n" ; //init expression and go to beggining
	ret->code << "\nlabel" << l1 << ": ; for.cond\n"; //for cond evaluation
	ret->code << "  br label %label" << l2 << "\n"; //direct jump command
	ret->code << "\nlabel" << l2 << ": ; for.body\n" << s1.code.str(); //for body evaluation
	ret->code << "  br label %label" << l3 << "\n" ; //go to inc
	ret->code << "\nlabel" << l3 << ": ; for.inc\n"; //for inc evaluation
	ret->code << "  br label %label" << l1 << "\n" ; //go to cond
	ret->code << "\nlabel" << l4 << ": ; for.end\n"; //end label
	return ret;
}


struct code_container* while_then(const struct expression& e1,
								  const struct code_container& s1) {
	struct code_container* ret;
	//WHILE is posible only if e1 is a boolean
	int l1, l2, l3;

	switch (e1.getT()) {
	case _BOOL:
		l1 = new_label();
		l2 = new_label();
		l3 = new_label();
		ret = new code_container();
		ret->code << "  br label %label" << l1 << "\n" ; //beginning label
		ret->code << "\nlabel" << l1 << ": ; while.cond\n" << e1.code.str(); //while cond evaluation
		ret->code << "  br i1 %x" << e1.getVar() << ", label %label" << l2 << ", label %label" << l3  << "\n"; //jump command
		ret->code << "\nlabel" << l2 << ": ; while.body\n" << s1.code.str(); //while body evaluation
		ret->code << "  br label %label" << l1 << "\n" ; //go to cond
		ret->code << "\nlabel" << l3 << ": ; while.end\n"; //end label
		break;

	case _INT:
		error_funct(_ERROR_COMPIL, "Can't use integer expression in an for statement");
		break;

	case _DOUBLE:
		error_funct(_ERROR_COMPIL, "Can't use double expression in an for statement");
		break;

	case _VOID:
		error_funct(_ERROR_COMPIL, "Can't use void expression in an for statement");
		break;

	case _ERROR:
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		break;

	default:
		// If you see this something really goes wrong withe the compiler
		error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
		break;

	}

	return ret;
}


struct code_container* do_while(const struct code_container& s1,
								const struct expression& e1) {
	struct code_container* ret;
	//DO WHILE is posible only if e1 is a boolean
	int l1, l2, l3;

	switch (e1.getT()) {
	case _BOOL:
		l1 = new_label();
		l2 = new_label();
		l3 = new_label();
		ret = new code_container();
		ret->code << "  br label %label" << l1 << "\n" ; //go to body
		ret->code << "\nlabel" << l1 << ": ; do.body\n" << s1.code.str(); //do body evaluation
		ret->code << "  br label %label" << l2 << "\n" ; //go to cond
		ret->code << "\nlabel" << l2 << ": ; do.cond\n" << e1.code.str(); //do cond evaluation
		ret->code << "  br i1 %x" << e1.getVar() << ", label %label" << l1 << ", label %label" << l3  << "\n"; //jump command
		ret->code << "\nlabel" << l3 << ": ; do.end\n"; //end label
		break;

	case _INT:
		error_funct(_ERROR_COMPIL, "Can't use integer expression in an for statement");
		break;

	case _DOUBLE:
		error_funct(_ERROR_COMPIL, "Can't use double expression in an for statement");
		break;

	case _VOID:
		error_funct(_ERROR_COMPIL, "Can't use void expression in an for statement");
		break;

	case _ERROR:
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		break;

	default:
		// If you see this something really goes wrong withe the compiler
		error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
		break;

	}

	return ret;
}


//Function to add p5 functions to the hash table in order to recognize them
void setup_p5(map_boost &hash) {
	// mathematicals functions
	struct identifier math;
	math.from_q5 = true;
	math.name = "@log10";
	math.symbolType = _FUNCTION;
	math.t = _DOUBLE;
	math.paramTypes.push_back(_DOUBLE);
	hash["log10"] = math;

	math.name = "@cos";
	hash["cos"] = math;

	math.name = "@sin";
	hash["sin"] = math;

	math.name = "@sqrt";
	hash["sqrt"] = math;

	math.t = _VOID;
	math.name = "@fill";
	hash["fill"] = math;

	math.name = "@background";
	hash["background"] = math;

	math.name = "@stroke";
	hash["stroke"] = math;

	math.paramTypes.push_back(_DOUBLE);
	math.name = "@createCanvas";
	hash["createCanvas"] = math;

	math.name = "@point";
	hash["point"] = math;

	math.paramTypes.push_back(_DOUBLE);
	math.paramTypes.push_back(_DOUBLE);
	math.name = "@line";
	hash["line"] = math;

	math.name = "@ellipse";
	hash["ellipse"] = math;

}


// used for function declaration
struct code_container* declare_q5_used_functions(map_boost &hash_table) {
	struct code_container *ret = new code_container();

	struct identifier id;
	BOOST_FOREACH(map_boost::value_type i, hash_table) {
		id = i.second;
		if (id.from_q5 && id.used && id.symbolType == _FUNCTION) {

			ret->code << "declare ";
			switch (id.t) {
			case _INT:
				ret->code << "i32 ";
				break;
			case _DOUBLE:
				ret->code << "double ";
				break;
			default:
				ret->code << "void ";
				break;
			}
			ret->code << id.name << "(";
			int count = 0;
			int size = id.paramTypes.size();
			BOOST_FOREACH(enum simple_type param_type, id.paramTypes) {

				count++;

				switch (param_type) {

				case _INT:
					ret->code << "i32";
					if (count < size) ret->code << ", ";
					break;

				case _DOUBLE:
					ret->code << "double";
					if (count < size) ret->code << ", ";
					break;

				default:
					cout << "ERROR" << endl;
					break;

				}

			}

			ret->code << ")\n";
		}
	}

	return ret;
}

struct code_container* declare_funct(enum simple_type t, string name, map_boost &hash_table){
	struct code_container *ret = new code_container();
	struct identifier id;

	id.symbolType = _FUNCTION;

	ret->code << "declare ";
	id.t = t;
	switch (t) {
	case _INT:
		ret->code << "i32 ";
		break;
	case _DOUBLE:
		ret->code << "double ";
		break;
	default:
		ret->code << "void ";
		break;
	}

	id.name = "@" + name;
	ret->code << id.name << "(";

	ret->code << ")\n";
	hash_table[name] = id;

	return ret;
}

struct code_container* declare_funct(enum simple_type t, string name, struct type_list tl, map_boost &hash_table) {
	struct code_container *ret = new code_container();
	struct identifier id;

	id.symbolType = _FUNCTION;

	ret->code << "declare ";
	id.t = t;
	switch (t) {
	case _INT:
		ret->code << "i32 ";
		break;
	case _DOUBLE:
		ret->code << "double ";
		break;
	default:
		ret->code << "void ";
		break;
	}

	id.name = "@" + name;
	ret->code << id.name << "(";

	int count = 0;
	int size = tl.list.size();
	BOOST_FOREACH(enum simple_type param_type, tl.list) {

		count++;

		id.paramTypes.push_back(param_type);
		switch (param_type) {

		case _INT:
			ret->code << "i32";
			if (count < size) ret->code << ", ";
			break;

		case _DOUBLE:
			ret->code << "double";
			if (count < size) ret->code << ", ";
			break;

		default:
			cout << "ERROR" << endl;
			break;

		}

	}

	ret->code << ")\n";
	hash_table[name] = id;

	return ret;
}


// declaration
struct code_container* local_declaration(enum simple_type t, struct declaration_list &decla, map_boost &hash_table) {
	struct code_container *ret = new code_container();

	struct identifier id; // will be use to insert it in the hash_table
	BOOST_FOREACH(identifier id_old, decla.idList) {
		//we check if symbol was not declared before
		if (hash_table.find(id_old.name) != hash_table.end()) {
			error_funct(_ERROR_COMPIL, "redeclaration of ", id_old.name);
		}
		else {

			switch (t) {

			case _INT:
				ret->code << "  %" << id_old.name << " = alloca i32\n";
				break;

			case _DOUBLE:
				ret->code << "  %" << id_old.name << " = alloca double\n";
				break;

			case _VOID:
				error_funct(_ERROR_COMPIL, "invalid use of void expression");
				break;

			case _BOOL:
				error_funct(_ERROR_COMPIL, "Canno't declare boolean typed variable");
				break;

			case _ERROR:
				// In this case, we consider that the expression has already an error and we don't consider others errors
				// in order to don't flood the error output with big expressions.
				break;

			default:
				// If you see this something really goes wrong withe the compiler
				error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
				break;

			};

			id.t = t;
			id.name = "%" + id_old.name;
			id.symbolType = _LOCAL_VAR;
			hash_table[id_old.name] = id;
		}
	}
	return ret;
}

// declaration
struct code_container* global_declaration(enum simple_type t, struct declaration_list &decla, map_boost &hash_table) {
	struct code_container *ret = new code_container();

	struct identifier id; // will be use to insert it in the hash_table
	BOOST_FOREACH(identifier id_old, decla.idList) {
		//we check if symbol was not declared before
		if (hash_table.find(id_old.name) != hash_table.end()) {
			error_funct(_ERROR_COMPIL, "redeclaration of ", id_old.name);
		}
		else {

			switch (t) {

			case _INT:
				ret->code << "@" << id_old.name << " = common global i32 0\n";
				break;

			case _DOUBLE:
				ret->code << "@" << id_old.name << " = common global double 0x000000000000000\n";
				break;

			case _VOID:
				error_funct(_ERROR_COMPIL, "invalid use of void expression");
				break;

			case _BOOL:
				error_funct(_ERROR_COMPIL, "Canno't declare boolean typed variable");
				break;

			case _ERROR:
				// In this case, we consider that the expression has already an error and we don't consider others errors
				// in order to don't flood the error output with big expressions.
				break;

			default:
				// If you see this something really goes wrong withe the compiler
				error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
				break;

			};

			id.t = t;
			id.name = "@" + id_old.name;
			id.symbolType = _LOCAL_VAR;
			hash_table[id_old.name] = id;
		}
	}
	return ret;
}


// return
struct code_container *return_statement(struct expression& e1) {
	struct code_container *ret = new code_container();
	ret->has_return = true;
	ret->code << e1.code.str();
	ret->code << "  ret ";
	switch (e1.getT()) {

	case _INT:
		ret->code << "i32 %x" << e1.getVar() << "\n";
		break;

	case _DOUBLE:
		ret->code << "double %x" << e1.getVar() << "\n";
		break;

	case _VOID:
		error_funct(_ERROR_COMPIL, "invalid use of void expression");
		break;

	case _BOOL:
		error_funct(_ERROR_COMPIL, "Canno't return boolean typed expression");
		break;

	case _ERROR:
		// In this case, we consider that the expression has already an error and we don't consider others errors
		// in order to don't flood the error output with big expressions.
		break;

	default:
		// If you see this something really goes wrong withe the compiler
		error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
		break;
	}
	return ret;
}


// Function definition
struct code_container *define_funct(enum simple_type t, string name, struct declaration_list *decla, struct code_container *code, map_boost &hash_table) {
	struct code_container *ret = new code_container();
	if (hash_table.find(name) != hash_table.end()) {
		error_funct(_ERROR_COMPIL, "redeclaration of ", name);
	}
	else {

		ret->code << "define ";
		switch (t) {

		case _INT:
			ret->code << "i32 ";
			break;

		case _DOUBLE:
			ret->code << "double ";
			break;

		case _VOID:
			ret->code << "void ";
			break;

		case _BOOL:
			error_funct(_ERROR_COMPIL, "A function canno't return a boolean");
			break;

		case _ERROR:
			// In this case, we consider that the expression has already an error and we don't consider others errors
			// in order to don't flood the error output with big expressions.
			break;

		default:
			// If you see this something really goes wrong withe the compiler
			error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
			break;

		}

		ret->code << "@" << name << " (" << decla->code.str() << ")\n" << "{\n";
		add_identifier(decla->idList, ret->code);
		ret->code << code->code.str();

		if (t != _VOID && code->has_return == false)
			error_funct(_ERROR_COMPIL, "control reaches end of non-void function");
		if (t == _VOID && code->has_return == false)
			ret->code << "  ret void\n";

		ret->code << "}\n\n";

		// add function name to the hash table
		struct identifier id;
		id.t = t;
		id.name = "@" + name;
		id.symbolType = _FUNCTION;
		BOOST_FOREACH(identifier id_old, decla->idList) {
			id.paramTypes.push_back(id_old.t);
		}
		hash_table[name] = id;
	}
	return ret;
}

struct code_container *define_funct(enum simple_type t, string name, struct code_container *code, map_boost &hash_table) {
	struct code_container *ret = new code_container();
	if (hash_table.find(name) != hash_table.end()) {
		error_funct(_ERROR_COMPIL, "redeclaration of ", name);
	}
	else {

		ret->code << "define ";
		switch (t) {

		case _INT:
			ret->code << "i32 ";
			break;

		case _DOUBLE:
			ret->code << "double ";
			break;

		case _VOID:
			ret->code << "void ";
			break;

		case _BOOL:
			error_funct(_ERROR_COMPIL, "A function canno't return a boolean");
			break;

		case _ERROR:
			// In this case, we consider that the expression has already an error and we don't consider others errors
			// in order to don't flood the error output with big expressions.
			break;

		default:
			// If you see this something really goes wrong withe the compiler
			error_funct(_ERROR_COMPIL, "if you see this something really goes wrong with the compiler");
			break;

		}

		ret->code << "@" << name << " ()\n" << "{\n";
		ret->code << code->code.str();

		if (t != _VOID && code->has_return == false)
			error_funct(_ERROR_COMPIL, "control reaches end of non-void function");
		if (t == _VOID && code->has_return == false)
			ret->code << "  ret void\n";

		ret->code << "}\n\n";

		// add function name to the hash table
		struct identifier id;
		id.t = t;
		id.name = "@" + name;
		id.symbolType = _FUNCTION;
		hash_table[name] = id;
	}
	return ret;
}
