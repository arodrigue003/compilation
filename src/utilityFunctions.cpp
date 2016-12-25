#include <string>
#include <iostream>
#include <sstream>

#include "utilityFunctions.hpp"

declaration_list::declaration_list(enum simple_type t, string name, map_boost &hash_table) {

	if (hash_table.find(name) != hash_table.end()) {
		// We don't let redifinition of an identifier occur
		error_funct(_ERROR_COMPIL, "Redefinition of identifier ", name);
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
		default:
			cout << "ERROR" << endl;
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
    union {
        double a;
        long long int b;
    } u;
    u.a = d;
    asprintf(&s, "%#08llx", u.b);
    return s;
}

void add_identifier(vector<identifier> to_store, stringstream& ss) {
    for (std::vector<identifier>::iterator it = to_store.begin(); it != to_store.end(); ++it) {
        struct identifier id;

        switch ((*it).t) {
        case _INT:
			ss << (*it).name << " = alloca i32\n";
            break;

        case _DOUBLE:
			ss << (*it).name << " = alloca double\n";
            break;

        default:
            cout << "ERROR\n";
            break;
        }

        switch ((*it).t) {
        case _INT:
			ss << "store i32 %x" << (*it).register_no << ", i32* " << (*it).name << "\n";
            break;

        case _DOUBLE:
			ss << "store double %x" << (*it).register_no << ", double* " << (*it).name << "\n";
            break;

        default:
            cout << "ERROR\n";
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
		break;
	}
	cerr << s << endl;
	has_error = true;
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
		break;
	}
	cerr << s1 << s2 << endl;
	has_error = true;
	return 0;
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
