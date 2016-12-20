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

void add_identifier(vector<identifier> to_store, struct code_container * cc) {
	for (std::vector<identifier>::iterator it = to_store.begin(); it != to_store.end(); ++it){
		struct identifier id;

		switch ((*it).t) {
		case _INT:
			cc->code << "%" << (*it).name << " = alloca i32\n";
			break;
		case _DOUBLE:
			cc->code << "%" << (*it).name << " = alloca double\n";
			break;
		default:
			cout << "ERROR\n";
			break;
		}

		switch ((*it).t) {
		case _INT:
			cc->code << "store i32 %x" << (*it).register_no << ", i32* %" << (*it).name << "\n";
			break;
		case _DOUBLE:
			cc->code << "store double %x" << (*it).register_no << ", double* %" << (*it).name << "\n";
			break;
		default:
			cout << "ERROR\n";
			break;
		}
	}
	to_store.clear();
}

// Identifier gestion
struct expression *load_identifier(char *s, map_boost hash) {
	struct expression *ret;
	if (hash.find(s) == hash.end()){
		ret = new expression(_VOID, -1);
		cerr << "Can't find identifier " << s << endl;
	}
	else {
		switch (hash.at(s).t) {
		case _INT:
			ret = new expression(_INT, new_var());
			ret->code << "%x" << ret->getVar() << " = load i32, i32* %" << s << "\n";
			break;
		case _DOUBLE:
			ret = new expression(_DOUBLE, new_var());
			ret->code << "%x" << ret->getVar() << " = load double, double* %" << s << "\n";
			break;
		default:
			cout << "ERROR\n";
			break;
		}
	}
	return ret;
}

// Code generation for basics operations between expressions
struct expression *expression_addition(struct expression * e1, struct expression * e2) {
	struct expression * ret;
	if (e1->getT() == _INT) {
		if (e2->getT() == _INT) {
			ret = new expression(_INT, new_var());
			ret->code << e1->code.str() << e2->code.str();
			ret->code << "%x" << ret->getVar() << " = add i32 %x" << e1->getVar() << ", %x" << e2->getVar() << "\n";
		}
		else if (e2->getT() == _DOUBLE) {
			int conversion = new_var();
			ret = new expression(_DOUBLE, new_var());
			ret->code << e1->code.str() << e2->code.str() << "%x" << conversion << " = sitofp i32 %x" << e1->getVar() << " to double\n";
			ret->code << "%x" << ret->getVar() << " = fadd double %x" << e2->getVar() << ", %x" << conversion << "\n";
		}
	}
	else if(e1->getT() == _DOUBLE) {
		if (e2->getT() == _INT) {
			int conversion = new_var();
			ret = new expression(_DOUBLE, new_var());
			ret->code << e1->code.str() << e2->code.str() << "%x" << conversion << " = sitofp i32 %x" << e2->getVar() << " to double\n";
			ret->code << "%x" << ret->getVar() << " = fadd double %x" << e1->getVar() << ", %x" << conversion << "\n";
		}
		else if (e2->getT() == _DOUBLE) {
			ret = new expression(_DOUBLE, new_var());
			ret->code << e1->code.str() << e2->code.str() << "%x" << ret->getVar() << " = fadd double %x" << e1->getVar() << ", %x" << e2->getVar() << "\n";
		}
	}
	return ret;
}

struct expression *expression_soustraction(struct expression * e1, struct expression * e2) {
	struct expression * ret;
	if (e1->getT() == _INT) {
		if (e2->getT() == _INT) {
			ret = new expression(_INT, new_var());
			ret->code << e1->code.str() << e2->code.str() << "%x" << ret->getVar() << " = sub i32 %x" << e1->getVar() << ", %x" << e2->getVar() << "\n";
		}
		else if (e2->getT() == _DOUBLE) {
			int conversion = new_var();
			ret = new expression(_DOUBLE, new_var());
			ret->code << e1->code.str() << e2->code.str() << "%x" << conversion << " = sitofp i32 %x" << e1->getVar() << " to double\n";
			ret->code << "%x" << ret->getVar() << " = fsub double %x" << e2->getVar() << ", %x" << conversion << "\n";
		}
	}
	else if(e1->getT() == _DOUBLE) {
		if (e2->getT() == _INT) {
			int conversion = new_var();
			ret = new expression(_DOUBLE, new_var());
			ret->code << e1->code.str() << e2->code.str() << "%x" << conversion << " = sitofp i32 %x" << e2->getVar() << " to double\n";
			ret->code << "%x" << ret->getVar() << " = fsub double %x" << e1->getVar() << ", %x" << conversion << "\n";
		}
		else if (e2->getT() == _DOUBLE) {
			ret = new expression(_DOUBLE, new_var());
			ret->code << e1->code.str() << e2->code.str() << "%x" << ret->getVar() << " = fadd double %x" << e1->getVar() << ", %x" << e2->getVar() << "\n";
			//asprintf(&ret->old, "%s%s%%x%d = fsub double %%x%d, %%x%d\n", e1->code, e2->code, ret->getVar(), e1->getVar(), e2->getVar());
		}
	}
	return ret;
}

struct expression *expression_multiplication(struct expression * e1, struct expression * e2) {
	struct expression * ret;
	if (e1->getT() == _INT) {
		if (e2->getT() == _INT) {
			ret = new expression(_INT, new_var());
			ret->code << e1->code.str() << e2->code.str() << "%x" << ret->getVar() << " = mul i32 %x" << e1->getVar() << ", %x" << e2->getVar() << "\n";
		}
		else if (e2->getT() == _DOUBLE) {
			int conversion = new_var();
			ret = new expression(_DOUBLE, new_var());
			ret->code << e1->code.str() << e2->code.str() << "%x" << conversion << " = sitofp i32 %x" << e1->getVar() << " to double\n";
			ret->code << "%x" << ret->getVar() << " = fmul double %x" << e2->getVar() << ", %x" << conversion << "\n";
		}
	}
	else if(e1->getT() == _DOUBLE) {
		if (e2->getT() == _INT) {
			int conversion = new_var();
			ret = new expression(_DOUBLE, new_var());
			ret->code << e1->code.str() << e2->code.str() << "%x" << conversion << " = sitofp i32 %x" << e2->getVar() << " to double\n";
			ret->code << "%x" << ret->getVar() << " = fmul double %x" << e1->getVar() << ", %x" << conversion << "\n";
		}
		else if (e2->getT() == _DOUBLE) {
			ret = new expression(_DOUBLE, new_var());
			ret->code << e1->code.str() << e2->code.str() << "%x" << ret->getVar() << " = fadd double %x" << e1->getVar() << ", %x" << e2->getVar() << "\n";
			//asprintf(&ret->old, "%s%s%%x%d = fmul double %%x%d, %%x%d\n", e1->code, e2->code, ret->getVar(), e1->getVar(), e2->getVar());
		}
	}
	return ret;
}

struct expression *expression_quotient(struct expression * e1, struct expression * e2) {
	struct expression * ret;
	if (e1->getT() == _INT) {
		if (e2->getT() == _INT) {
			ret = new expression(_INT, new_var());
			ret->code << e1->code.str() << e2->code.str() << "%x" << ret->getVar() << " = sdiv i32 %x" << e1->getVar() << ", %x" << e2->getVar() << "\n";
		}
		else if (e2->getT() == _DOUBLE) {
			int conversion = new_var();
			ret = new expression(_DOUBLE, new_var());
			ret->code << e1->code.str() << e2->code.str() << "%x" << conversion << " = sitofp i32 %x" << e1->getVar() << " to double\n";
			ret->code << "%x" << ret->getVar() << " = fdiv double %x" << e2->getVar() << ", %x" << conversion << "\n";
		}
	}
	else if(e1->getT() == _DOUBLE) {
		if (e2->getT() == _INT) {
			int conversion = new_var();
			ret = new expression(_DOUBLE, new_var());
			ret->code << e1->code.str() << e2->code.str() << "%x" << conversion << " = sitofp i32 %x" << e2->getVar() << " to double\n";
			ret->code << "%x" << ret->getVar() << " = fdiv double %x" << e1->getVar() << ", %x" << conversion << "\n";
		}
		else if (e2->getT() == _DOUBLE) {
			ret = new expression(_DOUBLE, new_var());
			ret->code << e1->code.str() << e2->code.str() << "%x" << ret->getVar() << " = fadd double %x" << e1->getVar() << ", %x" << e2->getVar() << "\n";
			//asprintf(&ret->old, "%s%s%%x%d = fdiv double %%x%d, %%x%d\n", e1->code, e2->code, ret->getVar(), e1->getVar(), e2->getVar());
		}
	}
	return ret;
}

struct expression *expression_reste(struct expression * e1, struct expression * e2) {
	struct expression * ret;
	if (e1->getT() == _INT) {
		if (e2->getT() == _INT) {
			ret = new expression(_INT, new_var());
			ret->code << e1->code.str() << e2->code.str() << "%x" << ret->getVar() << " = srem i32 %x" << e1->getVar() << ", %x" << e2->getVar() << "\n";
		}
		else if (e2->getT() == _DOUBLE) {
			int conversion = new_var();
			ret = new expression(_DOUBLE, new_var());
			ret->code << e1->code.str() << e2->code.str() << "%x" << conversion << " = sitofp i32 %x" << e1->getVar() << " to double\n";
			ret->code << "%x" << ret->getVar() << " = frem double %x" << e2->getVar() << ", %x" << conversion << "\n";
		}
	}
	else if(e1->getT() == _DOUBLE) {
		if (e2->getT() == _INT) {
			int conversion = new_var();
			ret = new expression(_DOUBLE, new_var());
			ret->code << e1->code.str() << e2->code.str() << "%x" << conversion << " = sitofp i32 %x" << e2->getVar() << " to double\n";
			ret->code << "%x" << ret->getVar() << " = frem double %x" << e1->getVar() << ", %x" << conversion << "\n";
		}
		else if (e2->getT() == _DOUBLE) {
			ret = new expression(_DOUBLE, new_var());
			ret->code << e1->code.str() << e2->code.str() << "%x" << ret->getVar() << " = fadd double %x" << e1->getVar() << ", %x" << e2->getVar() << "\n";
		}
	}
	return ret;
}


// Code generation for assignments
struct expression *assign_equal(struct expression * e1, char * s, map_boost hash){
	struct expression *ret;
	if (hash.find(s) == hash.end()) {
		ret = new expression(_VOID, -1);
		cerr << "Can't find identifier " << s << endl;
	}
	else {
		int var = e1->getVar();
		switch (hash.at(s).t) {
		case _INT:
			ret = new expression(_INT, -1);
			ret->code << e1->code.str();
			switch (e1->getT()) {
			case _INT:
				break;
			case _DOUBLE:
				// In this case expression mut be converted in an int value
				var = new_var();
				ret->code << "%x" << var <<  " = fptosi double %x" << e1->getVar() << " to i32\n";
				break;
			default:
				cout << "ERROR\n";
				break;
			}
			ret->code << "store i32 %x" << var << ", i32* %" << s << "\n";
			break;
		case _DOUBLE:
			ret = new expression(_DOUBLE, -1);
			ret->code << e1->code.str();
			switch (e1->getT()) {
			case _INT:
				// In this case expression mut be converted in an double value
				var = new_var();
				ret->code << "%x" << var << " = sitofp i32 %x" << e1->getVar() << " to double\n";
				break;
			case _DOUBLE:
				break;
			default:
				cout << "ERROR\n";
				break;
			}
			ret->code << "store double %x" << var << ", double* %" << s << "\n";
			break;
		default:
			cout << "ERROR\n";
			break;
		}
	}
	return ret;
}
