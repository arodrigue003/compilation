#include "expression_symbols.hpp"

#include <stdlib.h>
#include <string.h>




void declarator::add(char *name){
	names.push_back(name);
}

void declarator::add(string name){
	names.push_back(name);
}

void declarator::show(){
	std::cout << "myvector contains:";
	for (std::vector<string>::iterator it = names.begin() ; it != names.end(); ++it)
		std::cout << ' ' << *it;
	std::cout << '\n';
}

void declarator::merge(declarator *other){
	string tmp;
	for (std::vector<string>::iterator it = other->names.begin() ; it != other->names.end(); ++it){
		tmp = *it;
		this->add(tmp);
	}
}


std::vector<string>::iterator declarator::begin() {
	return names.begin();
}

std::vector<string>::iterator declarator::end(){
	return names.end();
}

