#include "statement.hpp"

struct code_container* if_statement(const struct expression &e1, const struct code_container &s1) {
	struct code_container * ret;

	//IF is posible only if expression is a boolean
	int l1, l2;
	switch (e1.getT()) {

	case _BOOL:
			l1 = new_label();
			l2 = new_label();
			ret = new code_container();

			ret->code << e1.code.str(); //Compute the expression
			ret->code << "br i1 %x" << e1.getVar() << ", label %label" << l1 << ", label %label" << l2 << "\n"; //jump command
			ret->code << "\nlabel" << l1 << ": ; if.then\n" << s1.code.str() << "br label %label" << l2 << "\n" ; //then command
			ret->code << "\nlabel" << l2 << ": ; if.end\n"; //ifend command
			break;

	default:
			cerr << "Wrong type for the IF" << endl;
			ret = new code_container();
			break;

	}
	return ret;
}

struct code_container* if_else_statement(const struct expression &e1, const struct code_container &s1, const struct code_container &s2) {
	struct code_container * ret;

	//IF is posible only if expression is a boolean
	int l1, l2, l3;
	switch (e1.getT()) {

	case _BOOL:
			l1 = new_label();
			l2 = new_label();
			l3 = new_label();
			ret = new code_container();

			ret->code << e1.code.str(); //Compute the expression
			ret->code << "br i1 %x" << e1.getVar() << ", label %label" << l1 << ", label %label" << l2 << "\n"; //jump command
			ret->code << "\nlabel" << l1 << ": ; if.then\n" << s1.code.str() << "br label %label" << l3 << "\n" ; //then command
			ret->code << "\nlabel" << l2 << ": ; if.else\n" << s2.code.str() << "br label %label" << l3 << "\n" ; //else command
			ret->code << "\nlabel" << l3 << ": ; if.end\n"; //ifend command
			break;

	default:
			cerr << "Wrong type for the IF" << endl;
			ret = new code_container();
			break;

	}
	return ret;
}


