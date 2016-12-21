#include <string>
#include <iostream>
#include <sstream>

#include "utilityFunctions.hpp"

int new_var() {
	static int i=0;
	return i++;
}

int new_label() {
	static int i=0;
	return i++;
}

char *double_to_hex_str(double d) {
	char *s = NULL;
	union {
		double a;
		long long int b;
	} u;
	u.a = d;
	asprintf(&s, "%#08llx", u.b);
	return s;
}

void add_identifier(vector<identifier> to_store, stringstream &ss) {
	for (std::vector<identifier>::iterator it = to_store.begin(); it != to_store.end(); ++it){
		struct identifier id;

		switch ((*it).t) {
		case _INT:
			ss << "%" << (*it).name << " = alloca i32\n";
			break;
		case _DOUBLE:
			ss << "%" << (*it).name << " = alloca double\n";
			break;
		default:
			cout << "ERROR\n";
			break;
		}

		switch ((*it).t) {
		case _INT:
			ss << "store i32 %x" << (*it).register_no << ", i32* %" << (*it).name << "\n";
			break;
		case _DOUBLE:
			ss << "store double %x" << (*it).register_no << ", double* %" << (*it).name << "\n";
			break;
		default:
			cout << "ERROR\n";
			break;
		}
	}
	to_store.clear();
}
