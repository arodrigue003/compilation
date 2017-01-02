#include <string>
#include <iostream>
#include <sstream>

#include "utilityFunctions.hpp"

declaration_list::declaration_list(enum simple_type t, string name, map_boost &hash_table) {

	if (hash_table.find(name) != hash_table.end()) {
		// We don't let redifinition of an identifier occur
		error_funct(_ERROR_COMPIL, "redeclaration ", name);
		return ;
	}

	int var = new_var();
	switch (t) {

	case _INT:
		code << "i32 %x" << var;
		break;

	case _DOUBLE:
		code << "double %x" << var;
		break;

	case _VOID:
		error_funct(_ERROR_COMPIL, "Invalid use of void expression");
		break;

	case _BOOL:
		error_funct(_ERROR_COMPIL, "Invalid use of boolean expression");
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

	struct identifier id;
	id.t = t;
	id.name = "%" + name;
	id.register_no = var;
	id.symbolType = _LOCAL_VAR;
	idList.push_back(id);

	hash_table[name] = id; //add variable to the global hash table variable
}







int new_var() {
	static int i = 0;
	return i++;
}

int new_label() {
	static int i = 0;
	return i++;
}

char* double_to_hex_str(double d) {
	char* s = nullptr;

	if (d != 0.0) {
		union {
			double a;
			long long int b;
		} u;
		u.a = d;
		asprintf(&s, "%#08llx", u.b);
	}
	else
		asprintf(&s, "0x000000000000000");
	return s;
}

void add_identifier(vector<identifier> &to_store, stringstream& ss) {

	BOOST_FOREACH(identifier id_old, to_store) {
		struct identifier id;

		switch (id_old.t) {
		case _INT:
			ss << "  " << id_old.name << " = alloca i32\n";
			break;

		case _DOUBLE:
			ss << "  " << id_old.name << " = alloca double\n";
			break;

		case _VOID:
			error_funct(_ERROR_COMPIL, "Invalid use of void expression");
			break;

		case _BOOL:
			error_funct(_ERROR_COMPIL, "Invalid use of boolean expression");
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

		switch (id_old.t) {
		case _INT:
			ss << "  store i32 %x" << id_old.register_no << ", i32* " << id_old.name << "\n";
			break;

		case _DOUBLE:
			ss << "  store double %x" << id_old.register_no << ", double* " << id_old.name << "\n";
			break;

		case _VOID:
			error_funct(_ERROR_COMPIL, "Invalid use of void expression");
			break;

		case _BOOL:
			error_funct(_ERROR_COMPIL, "Invalid use of boolean expression");
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
	}
}


extern char *file_name;
extern int column;
extern int yylineno;

int error_funct (enum error_type et, string s) {
	cerr << file_name << ":" << yylineno << ":" << column << ": ";
	switch (et) {

	case _WARNING_COMPIL:
		cerr << "warning: ";
		break;

	case _ERROR_COMPIL:
		cerr << "error: ";
		has_error = true;
		break;
	}
	cerr << s << endl;
	return 0;
}

int error_funct(enum error_type et, string s1, string s2) {
	cerr << file_name << ":" << yylineno << ":" << column << ": ";
	switch (et) {

	case _WARNING_COMPIL:
		cerr << "warning: ";
		break;

	case _ERROR_COMPIL:
		cerr << "error: ";
		has_error = true;
		break;
	}
	cerr << s1 << s2 << endl;
	return 0;
}


//Function to add p5 functions to the hash table in order to recognize them
void setup_p5(map_boost &hash) {
	// mathematicals functions
	struct identifier math;

	math = identifier(_VOID, "@noStroke");
	hash["noStroke"] = math;
	math = identifier(_VOID, "@noLoop");
	hash["noLoop"] = math;

	math = identifier(_DOUBLE, "@log10", _DOUBLE);
	hash["log10"] = math;
	math = identifier(_DOUBLE, "@cos", _DOUBLE);
	hash["cos"] = math;
	math = identifier(_DOUBLE, "@sin", _DOUBLE);
	hash["sin"] = math;
	math = identifier(_DOUBLE, "@sqrt", _DOUBLE);
	hash["sqrt"] = math;
	math = identifier(_DOUBLE, "@radians", _DOUBLE);
	hash["radians"] = math;


	math = identifier(_VOID, "@fill", _DOUBLE);
	hash["fill"] = math;
	math = identifier(_VOID, "@background", _DOUBLE);
	hash["background"] = math;
	math = identifier(_VOID, "@stroke", _DOUBLE);
	hash["stroke"] = math;
	math = identifier(_VOID, "@frameRate", _DOUBLE);
	hash["frameRate"] = math;
	math = identifier(_VOID, "@createCanvas", _DOUBLE, _DOUBLE);
	hash["createCanvas"] = math;

	math = identifier(_VOID, "@point", _DOUBLE, _DOUBLE);
	hash["point"] = math;

	math = identifier(_VOID, "@line", _DOUBLE, _DOUBLE, _DOUBLE, _DOUBLE);
	hash["line"] = math;
	math = identifier(_VOID, "@ellipse", _DOUBLE, _DOUBLE, _DOUBLE, _DOUBLE);
	hash["ellipse"] = math;
	math = identifier(_VOID, "@rect", _DOUBLE, _DOUBLE, _DOUBLE, _DOUBLE);
	hash["rect"] = math;
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
