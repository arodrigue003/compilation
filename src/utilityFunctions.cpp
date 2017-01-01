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
	to_store.clear();
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
