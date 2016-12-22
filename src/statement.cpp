#include "statement.hpp"

struct code_container* if_then_else(const struct expression &e1, const struct code_container &s1) {
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

struct code_container* if_then_else(const struct expression &e1, const struct code_container &s1, const struct code_container &s2) {
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


struct code_container* for_then(const struct expression &e1, const struct expression &e2, const struct expression &e3, const struct code_container &s1) {
	struct code_container *ret;
	//FOR is posible only if e2 is a boolean
	int l1, l2, l3, l4;
	switch (e2.getT()) {

	case _BOOL:
		l1 = new_label();
		l2 = new_label();
		l3 = new_label();
		l4 = new_label();
		ret = new code_container();

		ret->code << e1.code.str() << "br label %label" << l1 << "\n" ; //init expression and go to beggining
		ret->code << "\nlabel" << l1 << ": ; for.cond\n" << e2.code.str(); //for cond evaluation
		ret->code << "br i1 %x" << e2.getVar() << ", label %label" << l2 << ", label %label" << l4 << "\n"; //jump command
		ret->code << "\nlabel" << l2 << ": ; for.body\n" << s1.code.str(); //for body evaluation
		ret->code << "br label %label" << l3 << "\n" ; //go to inc
		ret->code << "\nlabel" << l3 << ": ; for.inc\n" << e3.code.str(); //for inc evaluation
		ret->code << "br label %label" << l1 << "\n" ; //go to cond
		ret->code << "\nlabel" << l4 << ": ; for.end\n"; //end label

		break;

	default:
		cerr << "Wrong type for the for code" << endl;
		ret = new code_container();
		break;

	}
	return ret;
}

struct code_container* for_then(const struct expression &e1, const struct expression &e2, void *, const struct code_container &s1) {
	struct code_container *ret;
	//FOR is posible only if e2 is a boolean
	int l1, l2, l3, l4;
	switch (e2.getT()) {

	case _BOOL:
		l1 = new_label();
		l2 = new_label();
		l3 = new_label();
		l4 = new_label();
		ret = new code_container();

		ret->code << e1.code.str() << "br label %label" << l1 << "\n" ; //init expression and go to beggining
		ret->code << "\nlabel" << l1 << ": ; for.cond\n" << e2.code.str(); //for cond evaluation
		ret->code << "br i1 %x" << e2.getVar() << ", label %label" << l2 << ", label %label" << l4 << "\n"; //jump command
		ret->code << "\nlabel" << l2 << ": ; for.body\n" << s1.code.str(); //for body evaluation
		ret->code << "br label %label" << l3 << "\n" ; //go to inc
		ret->code << "\nlabel" << l3 << ": ; for.inc\n"; //for inc evaluation
		ret->code << "br label %label" << l1 << "\n" ; //go to cond
		ret->code << "\nlabel" << l4 << ": ; for.end\n"; //end label

		break;

	default:
		cerr << "Wrong type for the for code" << endl;
		ret = new code_container();
		break;

	}
	return ret;
}

struct code_container* for_then(const struct expression &e1, void *, const struct expression &e3, const struct code_container &s1) {
	struct code_container *ret;
	// No need to comparaison because no test, always go to beggining
	int l1, l2, l3, l4;

	l1 = new_label();
	l2 = new_label();
	l3 = new_label();
	l4 = new_label();
	ret = new code_container();

	ret->code << e1.code.str() << "br label %label" << l1 << "\n" ; //init expression and go to beggining
	ret->code << "\nlabel" << l1 << ": ; for.cond\n"; //for cond evaluation
	ret->code << "br label %label" << l2 << "\n"; //direct jump command
	ret->code << "\nlabel" << l2 << ": ; for.body\n" << s1.code.str(); //for body evaluation
	ret->code << "br label %label" << l3 << "\n" ; //go to inc
	ret->code << "\nlabel" << l3 << ": ; for.inc\n" << e3.code.str(); //for inc evaluation
	ret->code << "br label %label" << l1 << "\n" ; //go to cond
	ret->code << "\nlabel" << l4 << ": ; for.end\n"; //end label


	return ret;
}

struct code_container* for_then(const struct expression &e1, void *, void *, const struct code_container &s1) {
	struct code_container *ret;
	// No need to comparaison because no test, always go to beggining
	int l1, l2, l3, l4;

	l1 = new_label();
	l2 = new_label();
	l3 = new_label();
	l4 = new_label();
	ret = new code_container();

	ret->code << e1.code.str() << "br label %label" << l1 << "\n" ; //init expression and go to beggining
	ret->code << "\nlabel" << l1 << ": ; for.cond\n"; //for cond evaluation
	ret->code << "br label %label" << l2 << "\n"; //direct jump command
	ret->code << "\nlabel" << l2 << ": ; for.body \n" << s1.code.str(); //for body evaluation
	ret->code << "br label %label" << l3 << "\n" ; //go to inc
	ret->code << "\nlabel" << l3 << ": ; for.inc\n"; //for inc evaluation
	ret->code << "br label %label" << l1 << "\n" ; //go to cond
	ret->code << "\nlabel" << l4 << ": ; for.end\n"; //end label

	return ret;
}

struct code_container* for_then(void *, const struct expression &e2, const struct expression &e3, const struct code_container &s1) {
	struct code_container *ret;
	//FOR is posible only if e2 is a boolean
	int l1, l2, l3, l4;
	switch (e2.getT()) {

	case _BOOL:
		l1 = new_label();
		l2 = new_label();
		l3 = new_label();
		l4 = new_label();
		ret = new code_container();

		ret->code << "br label %label" << l1 << "\n" ; //init expression and go to beggining
		ret->code << "\nlabel" << l1 << ": ; for.cond\n" << e2.code.str(); //for cond evaluation
		ret->code << "br i1 %x" << e2.getVar() << ", label %label" << l2 << ", label %label" << l4 << "\n"; //jump command
		ret->code << "\nlabel" << l2 << ": ; for.body\n" << s1.code.str(); //for body evaluation
		ret->code << "br label %label" << l3 << "\n" ; //go to inc
		ret->code << "\nlabel" << l3 << ": ; for.inc\n" << e3.code.str(); //for inc evaluation
		ret->code << "br label %label" << l1 << "\n" ; //go to cond
		ret->code << "\nlabel" << l4 << ": ; for.end\n"; //end label

		break;

	default:
		cerr << "Wrong type for the for code" << endl;
		ret = new code_container();
		break;

	}
	return ret;
}

struct code_container* for_then(void *, const struct expression &e2, void *, const struct code_container &s1) {
	struct code_container *ret;
	//FOR is posible only if e2 is a boolean
	int l1, l2, l3, l4;
	switch (e2.getT()) {

	case _BOOL:
		l1 = new_label();
		l2 = new_label();
		l3 = new_label();
		l4 = new_label();
		ret = new code_container();

		ret->code << "br label %label" << l1 << "\n" ; //init expression and go to beggining
		ret->code << "\nlabel" << l1 << ": ; for.cond\n" << e2.code.str(); //for cond evaluation
		ret->code << "br i1 %x" << e2.getVar() << ", label %label" << l2 << ", label %label" << l4 << "\n"; //jump command
		ret->code << "\nlabel" << l2 << ": ; for.body\n" << s1.code.str(); //for body evaluation
		ret->code << "br label %label" << l3 << "\n" ; //go to inc
		ret->code << "\nlabel" << l3 << ": ; for.inc\n"; //for inc evaluation
		ret->code << "br label %label" << l1 << "\n" ; //go to cond
		ret->code << "\nlabel" << l4 << ": ; for.end\n"; //end label

		break;

	default:
		cerr << "Wrong type for the for code" << endl;
		ret = new code_container();
		break;

	}
	return ret;
}

struct code_container* for_then(void *, void *, const struct expression &e3, const struct code_container &s1) {
	struct code_container *ret;
	// No need to comparaison because no test, always go to beggining
	int l1, l2, l3, l4;

	l1 = new_label();
	l2 = new_label();
	l3 = new_label();
	l4 = new_label();
	ret = new code_container();

	ret->code << "br label %label" << l1 << "\n" ; //init expression and go to beggining
	ret->code << "\nlabel" << l1 << ": ; for.cond\n"; //for cond evaluation
	ret->code << "br label %label" << l2 << "\n"; //direct jump command
	ret->code << "\nlabel" << l2 << ": ; for.body\n" << s1.code.str(); //for body evaluation
	ret->code << "br label %label" << l3 << "\n" ; //go to inc
	ret->code << "\nlabel" << l3 << ": ; for.inc\n" << e3.code.str(); //for inc evaluation
	ret->code << "br label %label" << l1 << "\n" ; //go to cond
	ret->code << "\nlabel" << l4 << ": ; for.end\n"; //end label


	return ret;
}

struct code_container* for_then(void *, void *, void *, const struct code_container &s1) {
	struct code_container *ret;
	// No need to comparaison because no test, always go to beggining
	int l1, l2, l3, l4;

	l1 = new_label();
	l2 = new_label();
	l3 = new_label();
	l4 = new_label();
	ret = new code_container();

	ret->code << "br label %label" << l1 << "\n" ; //init expression and go to beggining
	ret->code << "\nlabel" << l1 << ": ; for.cond\n"; //for cond evaluation
	ret->code << "br label %label" << l2 << "\n"; //direct jump command
	ret->code << "\nlabel" << l2 << ": ; for.body\n" << s1.code.str(); //for body evaluation
	ret->code << "br label %label" << l3 << "\n" ; //go to inc
	ret->code << "\nlabel" << l3 << ": ; for.inc\n"; //for inc evaluation
	ret->code << "br label %label" << l1 << "\n" ; //go to cond
	ret->code << "\nlabel" << l4 << ": ; for.end\n"; //end label


	return ret;
}

struct code_container* while_then(const struct expression &e1, const struct code_container &s1) {
	struct code_container *ret;
	//WHILE is posible only if e1 is a boolean
	int l1, l2, l3;
	switch (e1.getT()) {

	case _BOOL:
		l1 = new_label();
		l2 = new_label();
		l3 = new_label();
		ret = new code_container();

		ret->code << "br label %label" << l1 << "\n" ; //beginning label
		ret->code << "\nlabel" << l1 << ": ; while.cond\n" << e1.code.str(); //while cond evaluation
		ret->code << "br i1 %x" << e1.getVar() << ", label %label" << l2 << ", label %label" << l3 << "\n"; //jump command
		ret->code << "\nlabel" << l2 << ": ; while.body\n" << s1.code.str(); //while body evaluation
		ret->code << "br label %label" << l1 << "\n" ; //go to cond
		ret->code << "\nlabel" << l3 << ": ; while.end\n"; //end label

		break;

	default:
		cerr << "Wrong type for the for code" << endl;
		ret = new code_container();
		break;

	}
	return ret;
}

struct code_container* do_while(const struct code_container &s1, const struct expression &e1) {
	struct code_container *ret;
	//DO WHILE is posible only if e1 is a boolean
	int l1, l2, l3;
	switch (e1.getT()) {

	case _BOOL:
		l1 = new_label();
		l2 = new_label();
		l3 = new_label();
		ret = new code_container();

		ret->code << "br label %label" << l1 << "\n" ; //go to body
		ret->code << "\nlabel" << l1 << ": ; do.body\n" << s1.code.str(); //do body evaluation
		ret->code << "br label %label" << l2 << "\n" ; //go to cond
		ret->code << "\nlabel" << l2 << ": ; do.cond\n" << e1.code.str(); //do cond evaluation
		ret->code << "br i1 %x" << e1.getVar() << ", label %label" << l1 << ", label %label" << l3 << "\n"; //jump command
		ret->code << "\nlabel" << l3 << ": ; do.end\n"; //end label

		break;

	default:
		cerr << "Wrong type for the for code" << endl;
		ret = new code_container();
		break;

	}
	return ret;
}
