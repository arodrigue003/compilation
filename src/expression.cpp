#include "expression.hpp"

expression::expression(simple_type t, int var, map_boost& hash) : hash_table(hash), t(t),
    var(var) {}

// primary expression creation
expression::expression(char* s, map_boost& hash) : hash_table(hash) {
    if (hash.find(s) == hash.end()) {
        t = _ERROR;
        var = -1;
        cerr << "Can't find identifier " << s << endl;
        return;
    }
    else {
        switch (hash.at(s).t) {
        case _INT:
            t = _INT;
            var = new_var();
            code << "%x" << var << " = load i32, i32* %" << s << "\n";
            break;

        case _DOUBLE:
            t = _DOUBLE;
            var = new_var();
            code << "%x" << var << " = load double, double* %" << s << "\n";
            break;

        default:
            t = _ERROR;
            var = -1;
            cerr << "Wrong type for " << s << endl;
            break;
        }
    }
}

expression::expression(int i, map_boost& hash) : hash_table(hash), t(_INT),
    var(new_var()) {
    code << "%x" << var << " = add i32 0, " << i << "\n";
}

expression::expression(double d, map_boost& hash) : hash_table(hash), t(_DOUBLE),
    var(new_var()) {
    char* nb_double = double_to_hex_str(d);
    code << "%x" << var << " = fadd double 0x000000000000000, " << nb_double << "\n";
    free(nb_double);
}


// Getters
simple_type expression::getT() const {
    return t;
}

int expression::getVar() const {
    return var;
}

map_boost& expression::getHash() const  {
    return hash_table;
}


// Setters
void expression::setVar(int var) {
    this->var = var;
}


expression::~expression() {
}


// Code generation for basics operations between expressions
struct expression* operator+(const struct expression& e1, const struct expression& e2) {
    struct expression* ret;

    if (e1.t == _INT) {
        if (e2.t == _INT) {
            ret = new expression(_INT, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << ret->getVar() << " = add i32 %x" << e1.var << ", %x" << e2.var <<
                      "\n";
        }
        else if (e2.t == _DOUBLE) {
            int conversion = new_var();
            ret = new expression(_DOUBLE, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str() << "%x" << conversion << " = sitofp i32 %x" <<
                      e1.var << " to double\n";
            ret->code << "%x" << ret->getVar() << " = fadd double %x" << e2.var << ", %x" <<
                      conversion << "\n";
        }
    }
    else if (e1.t == _DOUBLE) {
        if (e2.t == _INT) {
            int conversion = new_var();
            ret = new expression(_DOUBLE, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str() << "%x" << conversion << " = sitofp i32 %x" <<
                      e2.var << " to double\n";
            ret->code << "%x" << ret->getVar() << " = fadd double %x" << e1.var << ", %x" <<
                      conversion << "\n";
        }
        else if (e2.t == _DOUBLE) {
            ret = new expression(_DOUBLE, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str() << "%x" << ret->getVar() <<
                      " = fadd double %x" << e1.var << ", %x" << e2.var << "\n";
        }
    }

    return ret;
}

struct expression* operator-(const struct expression& e1, const struct expression& e2) {
    struct expression* ret;

    if (e1.t == _INT) {
        if (e2.t == _INT) {
            ret = new expression(_INT, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str() << "%x" << ret->getVar() << " = sub i32 %x" <<
                      e1.var << ", %x" << e2.var << "\n";
        }
        else if (e2.t == _DOUBLE) {
            int conversion = new_var();
            ret = new expression(_DOUBLE, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str() << "%x" << conversion << " = sitofp i32 %x" <<
                      e1.var << " to double\n";
            ret->code << "%x" << ret->getVar() << " = fsub double %x" << e2.var << ", %x" <<
                      conversion << "\n";
        }
    }
    else if (e1.t == _DOUBLE) {
        if (e2.t == _INT) {
            int conversion = new_var();
            ret = new expression(_DOUBLE, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str() << "%x" << conversion << " = sitofp i32 %x" <<
                      e2.var << " to double\n";
            ret->code << "%x" << ret->getVar() << " = fsub double %x" << e1.var << ", %x" <<
                      conversion << "\n";
        }
        else if (e2.t == _DOUBLE) {
            ret = new expression(_DOUBLE, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str() << "%x" << ret->getVar() <<
                      " = fsub double %x" << e1.var << ", %x" << e2.var << "\n";
        }
    }

    return ret;
}

struct expression* operator*(const struct expression& e1, const struct expression& e2) {
    struct expression* ret;

    if (e1.t == _INT) {
        if (e2.t == _INT) {
            ret = new expression(_INT, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str() << "%x" << ret->getVar() << " = mul i32 %x" <<
                      e1.var << ", %x" << e2.var << "\n";
        }
        else if (e2.t == _DOUBLE) {
            int conversion = new_var();
            ret = new expression(_DOUBLE, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str() << "%x" << conversion << " = sitofp i32 %x" <<
                      e1.var << " to double\n";
            ret->code << "%x" << ret->getVar() << " = fmul double %x" << e2.var << ", %x" <<
                      conversion << "\n";
        }
    }
    else if (e1.t == _DOUBLE) {
        if (e2.t == _INT) {
            int conversion = new_var();
            ret = new expression(_DOUBLE, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str() << "%x" << conversion << " = sitofp i32 %x" <<
                      e2.var << " to double\n";
            ret->code << "%x" << ret->getVar() << " = fmul double %x" << e1.var << ", %x" <<
                      conversion << "\n";
        }
        else if (e2.t == _DOUBLE) {
            ret = new expression(_DOUBLE, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str() << "%x" << ret->getVar() <<
                      " = fmul double %x" << e1.var << ", %x" << e2.var << "\n";
        }
    }

    return ret;
}

struct expression* operator/(const struct expression& e1, const struct expression& e2) {
    struct expression* ret;

    if (e1.t == _INT) {
        if (e2.t == _INT) {
            ret = new expression(_INT, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str() << "%x" << ret->getVar() << " = sdiv i32 %x"
                      << e1.var << ", %x" << e2.var << "\n";
        }
        else if (e2.t == _DOUBLE) {
            int conversion = new_var();
            ret = new expression(_DOUBLE, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str() << "%x" << conversion << " = sitofp i32 %x" <<
                      e1.var << " to double\n";
            ret->code << "%x" << ret->getVar() << " = fdiv double %x" << e2.var << ", %x" <<
                      conversion << "\n";
        }
    }
    else if (e1.t == _DOUBLE) {
        if (e2.t == _INT) {
            int conversion = new_var();
            ret = new expression(_DOUBLE, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str() << "%x" << conversion << " = sitofp i32 %x" <<
                      e2.var << " to double\n";
            ret->code << "%x" << ret->getVar() << " = fdiv double %x" << e1.var << ", %x" <<
                      conversion << "\n";
        }
        else if (e2.t == _DOUBLE) {
            ret = new expression(_DOUBLE, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str() << "%x" << ret->getVar() <<
                      " = fdiv double %x" << e1.var << ", %x" << e2.var << "\n";
        }
    }

    return ret;
}

struct expression* operator%(const struct expression& e1, const struct expression& e2) {
    struct expression* ret;

    if (e1.t == _INT) {
        if (e2.t == _INT) {
            ret = new expression(_INT, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str() << "%x" << ret->getVar() << " = srem i32 %x"
                      << e1.var << ", %x" << e2.var << "\n";
        }
        else if (e2.t == _DOUBLE) {
            int conversion = new_var();
            ret = new expression(_DOUBLE, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str() << "%x" << conversion << " = sitofp i32 %x" <<
                      e1.var << " to double\n";
            ret->code << "%x" << ret->getVar() << " = frem double %x" << e2.var << ", %x" <<
                      conversion << "\n";
        }
    }
    else if (e1.t == _DOUBLE) {
        if (e2.t == _INT) {
            int conversion = new_var();
            ret = new expression(_DOUBLE, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str() << "%x" << conversion << " = sitofp i32 %x" <<
                      e2.var << " to double\n";
            ret->code << "%x" << ret->getVar() << " = frem double %x" << e1.var << ", %x" <<
                      conversion << "\n";
        }
        else if (e2.t == _DOUBLE) {
            ret = new expression(_DOUBLE, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str() << "%x" << ret->getVar() <<
                      " = frem double %x" << e1.var << ", %x" << e2.var << "\n";
        }
    }

    return ret;
}

struct expression* operator<<(const struct expression& e1, const struct expression& e2) {
    struct expression* ret;

    switch (e1.t) {
    case _INT:
        int conversion;

        switch (e2.t) {
        case _INT:
            ret = new expression(_INT, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << ret->getVar() << " = shl i32 %x" << e1.var << ", %x" << e2.var <<
                      "\n";
            break;

        case _DOUBLE:
            // In this cas we made an implicit convertion for e2 from double to int
            conversion = new_var();
            ret = new expression(_INT, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << conversion <<  " = fptosi double %x" << e2.var << " to i32\n";
            ret->code << "%x" << ret->getVar() << " = shl i32 %x" << e1.var << ", %x" << conversion <<
                      "\n";
            break;

        default:
            cerr << "Wrong type for the expression";
            ret = new expression(_ERROR, -1, e1.hash_table);
            break;
        }

        break;

    case _DOUBLE:
        cerr << "SHL operation canno't be applied to a double expression";
        ret = new expression(_ERROR, -1, e1.hash_table);
        break;

    default:
        cerr << "Wrong type for the expression";
        ret = new expression(_ERROR, -1, e1.hash_table);
        break;
    }

    return ret;
}

struct expression* operator>>(const struct expression& e1, const struct expression& e2) {
    struct expression* ret;

    switch (e1.t) {
    case _INT:
        int conversion;

        switch (e2.t) {
        case _INT:
            ret = new expression(_INT, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << ret->getVar() << " = ashr i32 %x" << e1.var << ", %x" << e2.var <<
                      "\n";
            break;

        case _DOUBLE:
            // In this cas we made an implicit convertion for e2 from double to int
            conversion = new_var();
            ret = new expression(_INT, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << conversion <<  " = fptosi double %x" << e2.var << " to i32\n";
            ret->code << "%x" << ret->getVar() << " = ashr i32 %x" << e1.var << ", %x" << conversion
                      << "\n";
            break;

        default:
            cerr << "Wrong type for the expression";
            ret = new expression(_ERROR, -1, e1.hash_table);
            break;
        }

        break;

    case _DOUBLE:
        cerr << "SHL operation canno't be applied to a double expression";
        ret = new expression(_ERROR, -1, e1.hash_table);
        break;

    default:
        cerr << "Wrong type for the expression";
        ret = new expression(_ERROR, -1, e1.hash_table);
        break;
    }

    return ret;
}


// Code generation for assignements
struct expression* expression::operator=(char* s) {
    struct expression* ret;

    if (hash_table.find(s) == hash_table.end()) {
        ret = new expression(_ERROR, -1, hash_table);
        cerr << "Can't find identifier " << s << endl;
    }
    else {
        int newVar = var;

        switch (hash_table.at(s).t) {
        case _INT:
            switch (t) {
            case _INT:
                ret = new expression(_INT, newVar, hash_table);
                ret->code << code.str();
                ret->code << "store i32 %x" << newVar << ", i32* %" << s << "\n";
                break;

            case _DOUBLE:
                // In this case expression mut be converted in an int value
                newVar = new_var();
                ret = new expression(_INT, newVar, hash_table);
                ret->code << code.str();
                ret->code << "%x" << newVar <<  " = fptosi double %x" << var << " to i32\n";
                ret->code << "store i32 %x" << newVar << ", i32* %" << s << "\n";
                break;

            default:
                ret = new expression(_ERROR, -1, hash_table);
                cerr << "Wrong type for " << s << endl;
                break;
            }

            break;

        case _DOUBLE:
            switch (t) {
            case _INT:
                // In this case expression mut be converted in an double value
                newVar = new_var();
                ret = new expression(_DOUBLE, newVar, hash_table);
                ret->code << code.str();
                ret->code << "%x" << newVar << " = sitofp i32 %x" << var << " to double\n";
                ret->code << "store double %x" << newVar << ", double* %" << s << "\n";
                break;

            case _DOUBLE:
                ret = new expression(_DOUBLE, newVar, hash_table);
                ret->code << code.str();
                ret->code << "store double %x" << newVar << ", double* %" << s << "\n";
                break;

            default:
                ret = new expression(_ERROR, -1, hash_table);
                cerr << "Wrong type for " << s << endl;
                break;
            }

            break;

        default:
            cout << "ERROR\n";
            break;
        }
    }

    return ret;
}

struct expression* expression::operator+=(char* s) {
    struct expression* ret;
    struct expression* e1 = new expression(s, hash_table);
    struct expression* e2 = *e1 + *this;
    ret = (*e2 = s);
    delete e1;
    e1 = nullptr;
    delete e2;
    e2 = nullptr;
    return ret;
}

struct expression* expression::operator-=(char* s) {
    struct expression* ret;
    struct expression* e1 = new expression(s, hash_table);
    struct expression* e2 = *e1 - *this;
    ret = (*e2 = s);
    delete e1;
    e1 = nullptr;
    delete e2;
    e2 = nullptr;
    return ret;
}

struct expression* expression::operator*=(char* s) {
    struct expression* ret;
    struct expression* e1 = new expression(s, hash_table);
    struct expression* e2 = *e1** this;
    ret = (*e2 = s);
    delete e1;
    e1 = nullptr;
    delete e2;
    e2 = nullptr;
    return ret;
}

struct expression* expression::operator/=(char* s) {
    struct expression* ret;
    struct expression* e1 = new expression(s, hash_table);
    struct expression* e2 = *e1 / *this;
    ret = (*e2 = s);
    delete e1;
    e1 = nullptr;
    delete e2;
    e2 = nullptr;
    return ret;
}

struct expression* expression::operator%=(char* s) {
    struct expression* ret;
    struct expression* e1 = new expression(s, hash_table);
    struct expression* e2 = *e1 % *this;
    ret = (*e2 = s);
    delete e1;
    e1 = nullptr;
    delete e2;
    e2 = nullptr;
    return ret;
}

struct expression* expression::operator<<=(char* s) {
    struct expression* ret;
    struct expression* e1 = new expression(s, hash_table);
    struct expression* e2 = *e1 << *this;
    ret = (*e2 = s);
    delete e1;
    e1 = nullptr;
    delete e2;
    e2 = nullptr;
    return ret;
}

struct expression* expression::operator>>=(char* s) {
    struct expression* ret;
    struct expression* e1 = new expression(s, hash_table);
    struct expression* e2 = *e1 >> *this;
    ret = (*e2 = s);
    delete e1;
    e1 = nullptr;
    delete e2;
    e2 = nullptr;
    return ret;
}


// Code generation for conditionals expression
struct expression* operator==(const struct expression& e1, const struct expression& e2) {
    struct expression* ret;
    int conversion;

    switch (e1.t) {
    case _INT:
        switch (e2.t) {
        case _INT:
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << ret->getVar() << " = icmp eq i32 %x" << e1.var << ", %x" << e2.var <<
                      "\n";
            break;

        case _DOUBLE:
            // In this cas we made an implicit convertion for e2 from int to double
            conversion = new_var();
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << conversion << " = sitofp i32 %x" << e1.var << " to double\n";
            ret->code << "%x" << ret->getVar() << " = fcmp oeq double %x" << conversion << ", %x" <<
                      e2.var << "\n";
            break;

        default:
            cerr << "Wrong type for the expression";
            ret = new expression(_ERROR, -1, e1.hash_table);
            break;
        }

        break;

    case _DOUBLE:
        switch (e2.t) {
        case _INT:
            // In this cas we made an implicit convertion for e1 from int to double
            conversion = new_var();
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << conversion << " = sitofp i32 %x" << e2.var << " to double\n";
            ret->code << "%x" << ret->getVar() << " = fcmp oeq double %x" << e1.var << ", %x" <<
                      conversion << "\n";
            break;

        case _DOUBLE:
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << ret->getVar() << " = fcmp oeq double %x" << e1.var << ", %x" <<
                      e2.var << "\n";
            break;

        default:
            cerr << "Wrong type for the expression";
            ret = new expression(_ERROR, -1, e1.hash_table);
            break;
        }

        break;

    default:
        cerr << "Wrong type for the expression";
        ret = new expression(_ERROR, -1, e1.hash_table);
        break;
    }

    return ret;
}

struct expression* operator!=(const struct expression& e1, const struct expression& e2) {
    struct expression* ret;
    int conversion;

    switch (e1.t) {
    case _INT:
        switch (e2.t) {
        case _INT:
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << ret->getVar() << " = icmp ne i32 %x" << e1.var << ", %x" << e2.var <<
                      "\n";
            break;

        case _DOUBLE:
            // In this cas we made an implicit convertion for e2 from int to double
            conversion = new_var();
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << conversion << " = sitofp i32 %x" << e1.var << " to double\n";
            ret->code << "%x" << ret->getVar() << " = fcmp one double %x" << conversion << ", %x" <<
                      e2.var << "\n";
            break;

        default:
            cerr << "Wrong type for the expression";
            ret = new expression(_ERROR, -1, e1.hash_table);
            break;
        }

        break;

    case _DOUBLE:
        switch (e2.t) {
        case _INT:
            // In this cas we made an implicit convertion for e1 from int to double
            conversion = new_var();
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << conversion << " = sitofp i32 %x" << e2.var << " to double\n";
            ret->code << "%x" << ret->getVar() << " = fcmp one double %x" << e1.var << ", %x" <<
                      conversion << "\n";
            break;

        case _DOUBLE:
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << ret->getVar() << " = fcmp one double %x" << e1.var << ", %x" <<
                      e2.var << "\n";
            break;

        default:
            cerr << "Wrong type for the expression";
            ret = new expression(_ERROR, -1, e1.hash_table);
            break;
        }

        break;

    default:
        cerr << "Wrong type for the expression";
        ret = new expression(_ERROR, -1, e1.hash_table);
        break;
    }

    return ret;
}

struct expression* operator<(const struct expression& e1, const struct expression& e2) {
    struct expression* ret;
    int conversion;

    switch (e1.t) {
    case _INT:
        switch (e2.t) {
        case _INT:
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << ret->getVar() << " = icmp slt i32 %x" << e1.var << ", %x" << e2.var
                      << "\n";
            break;

        case _DOUBLE:
            // In this cas we made an implicit convertion for e2 from int to double
            conversion = new_var();
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << conversion << " = sitofp i32 %x" << e1.var << " to double\n";
            ret->code << "%x" << ret->getVar() << " = fcmp olt double %x" << conversion << ", %x" <<
                      e2.var << "\n";
            break;

        default:
            cerr << "Wrong type for the expression";
            ret = new expression(_ERROR, -1, e1.hash_table);
            break;
        }

        break;

    case _DOUBLE:
        switch (e2.t) {
        case _INT:
            // In this cas we made an implicit convertion for e1 from int to double
            conversion = new_var();
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << conversion << " = sitofp i32 %x" << e2.var << " to double\n";
            ret->code << "%x" << ret->getVar() << " = fcmp olt double %x" << e1.var << ", %x" <<
                      conversion << "\n";
            break;

        case _DOUBLE:
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << ret->getVar() << " = fcmp olt double %x" << e1.var << ", %x" <<
                      e2.var << "\n";
            break;

        default:
            cerr << "Wrong type for the expression";
            ret = new expression(_ERROR, -1, e1.hash_table);
            break;
        }

        break;

    default:
        cerr << "Wrong type for the expression";
        ret = new expression(_ERROR, -1, e1.hash_table);
        break;
    }

    return ret;
}

struct expression* operator>(const struct expression& e1, const struct expression& e2) {
    struct expression* ret;
    int conversion;

    switch (e1.t) {
    case _INT:
        switch (e2.t) {
        case _INT:
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << ret->getVar() << " = icmp sgt i32 %x" << e1.var << ", %x" << e2.var
                      << "\n";
            break;

        case _DOUBLE:
            // In this cas we made an implicit convertion for e2 from int to double
            conversion = new_var();
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << conversion << " = sitofp i32 %x" << e1.var << " to double\n";
            ret->code << "%x" << ret->getVar() << " = fcmp ogt double %x" << conversion << ", %x" <<
                      e2.var << "\n";
            break;

        default:
            cerr << "Wrong type for the expression";
            ret = new expression(_ERROR, -1, e1.hash_table);
            break;
        }

        break;

    case _DOUBLE:
        switch (e2.t) {
        case _INT:
            // In this cas we made an implicit convertion for e1 from int to double
            conversion = new_var();
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << conversion << " = sitofp i32 %x" << e2.var << " to double\n";
            ret->code << "%x" << ret->getVar() << " = fcmp ogt double %x" << e1.var << ", %x" <<
                      conversion << "\n";
            break;

        case _DOUBLE:
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << ret->getVar() << " = fcmp ogt double %x" << e1.var << ", %x" <<
                      e2.var << "\n";
            break;

        default:
            cerr << "Wrong type for the expression";
            ret = new expression(_ERROR, -1, e1.hash_table);
            break;
        }

        break;

    default:
        cerr << "Wrong type for the expression";
        ret = new expression(_ERROR, -1, e1.hash_table);
        break;
    }

    return ret;
}

struct expression* operator<=(const struct expression& e1, const struct expression& e2) {
    struct expression* ret;
    int conversion;

    switch (e1.t) {
    case _INT:
        switch (e2.t) {
        case _INT:
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << ret->getVar() << " = icmp sle i32 %x" << e1.var << ", %x" << e2.var
                      << "\n";
            break;

        case _DOUBLE:
            // In this cas we made an implicit convertion for e2 from int to double
            conversion = new_var();
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << conversion << " = sitofp i32 %x" << e1.var << " to double\n";
            ret->code << "%x" << ret->getVar() << " = fcmp ole double %x" << conversion << ", %x" <<
                      e2.var << "\n";
            break;

        default:
            cerr << "Wrong type for the expression";
            ret = new expression(_ERROR, -1, e1.hash_table);
            break;
        }

        break;

    case _DOUBLE:
        switch (e2.t) {
        case _INT:
            // In this cas we made an implicit convertion for e1 from int to double
            conversion = new_var();
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << conversion << " = sitofp i32 %x" << e2.var << " to double\n";
            ret->code << "%x" << ret->getVar() << " = fcmp ole double %x" << e1.var << ", %x" <<
                      conversion << "\n";
            break;

        case _DOUBLE:
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << ret->getVar() << " = fcmp ole double %x" << e1.var << ", %x" <<
                      e2.var << "\n";
            break;

        default:
            cerr << "Wrong type for the expression";
            ret = new expression(_ERROR, -1, e1.hash_table);
            break;
        }

        break;

    default:
        cerr << "Wrong type for the expression";
        ret = new expression(_ERROR, -1, e1.hash_table);
        break;
    }

    return ret;
}

struct expression* operator>=(const struct expression& e1, const struct expression& e2) {
    struct expression* ret;
    int conversion;

    switch (e1.t) {
    case _INT:
        switch (e2.t) {
        case _INT:
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << ret->getVar() << " = icmp sge i32 %x" << e1.var << ", %x" << e2.var
                      << "\n";
            break;

        case _DOUBLE:
            // In this cas we made an implicit convertion for e2 from int to double
            conversion = new_var();
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << conversion << " = sitofp i32 %x" << e1.var << " to double\n";
            ret->code << "%x" << ret->getVar() << " = fcmp oge double %x" << conversion << ", %x" <<
                      e2.var << "\n";
            break;

        default:
            cerr << "Wrong type for the expression";
            ret = new expression(_ERROR, -1, e1.hash_table);
            break;
        }

        break;

    case _DOUBLE:
        switch (e2.t) {
        case _INT:
            // In this cas we made an implicit convertion for e1 from int to double
            conversion = new_var();
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << conversion << " = sitofp i32 %x" << e2.var << " to double\n";
            ret->code << "%x" << ret->getVar() << " = fcmp oge double %x" << e1.var << ", %x" <<
                      conversion << "\n";
            break;

        case _DOUBLE:
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << ret->getVar() << " = fcmp oge double %x" << e1.var << ", %x" <<
                      e2.var << "\n";
            break;

        default:
            cerr << "Wrong type for the expression";
            ret = new expression(_ERROR, -1, e1.hash_table);
            break;
        }

        break;

    default:
        cerr << "Wrong type for the expression";
        ret = new expression(_ERROR, -1, e1.hash_table);
        break;
    }

    return ret;
}


// Code generation for logicales operators
struct expression* operator&&(const struct expression& e1, const struct expression& e2) {
    struct expression* ret;

    switch (e1.t) {
    case _BOOL:
        switch (e2.t) {
        case _BOOL:
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << ret->getVar() << " = and i1 %x" << e1.var << ", %x" << e2.var <<
                      "\n";
            break;

        default:
            cerr << "Wrong type for the expression" << endl;
            ret = new expression(_ERROR, -1, e1.hash_table);
            break;
        }

        break;

    default:
        cerr << "Wrong type for the expression" << endl;
        ret = new expression(_ERROR, -1, e1.hash_table);
        break;
    }

    return ret;
}

struct expression* operator||(const struct expression& e1, const struct expression& e2) {
    struct expression* ret;

    switch (e1.t) {
    case _BOOL:
        switch (e2.t) {
        case _BOOL:
            ret = new expression(_BOOL, new_var(), e1.hash_table);
            ret->code << e1.code.str() << e2.code.str();
            ret->code << "%x" << ret->getVar() << " = or i1 %x" << e1.var << ", %x" << e2.var << "\n";
            break;

        default:
            cerr << "Wrong type for the expression" << endl;
            ret = new expression(_ERROR, -1, e1.hash_table);
            break;
        }

        break;

    default:
        cerr << "Wrong type for the expression" << endl;
        ret = new expression(_ERROR, -1, e1.hash_table);
        break;
    }

    return ret;
}

struct expression* operator!(const struct expression& e1) {
    struct expression* ret;

    switch (e1.t) {
    case _BOOL:
        ret = new expression(_BOOL, new_var(), e1.hash_table);
        ret->code << e1.code.str();
        ret->code << "%x" << ret->getVar() << " = xor i1 %x" << e1.var << ", 1\n";
        break;

    default:
        cerr << "Wrong type for the expression" << endl;
        ret = new expression(_ERROR, -1, e1.hash_table);
        break;
    }

    return ret;
}
