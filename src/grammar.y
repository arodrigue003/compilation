%{
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif //_GNU_SOURCE
#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <boost/unordered_map.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <vector>
#include "utilityFunctions.hpp"
#include "expression.hpp"
#include "statement.hpp"

using namespace std;

// headers definition to permit compilation
extern int yylineno;
int yyerror(const char *s);

extern "C"
{
    int yyparse(void);
    int yylex(void);
    int yywrap()
    {
        return 1;
    }
}

int indentation_lvl = 0;

//Hash map
map_boost global_hash_table;

//utility functions
stringstream code;

%}

%define parse.error verbose

%token <string_c> IDENTIFIER
%token <n> CONSTANTI
%token <d> CONSTANTD
%token INC_OP DEC_OP LE_OP GE_OP EQ_OP NE_OP
%token SUB_ASSIGN MUL_ASSIGN ADD_ASSIGN DIV_ASSIGN
%token SHL_ASSIGN SHR_ASSIGN
%token REM_ASSIGN
%token REM SHL SHR
%token AND OR
%token TYPE_NAME
%token INT DOUBLE VOID
%token IF ELSE DO WHILE RETURN FOR
%type <s> conditional_expression logical_or_expression logical_and_expression shift_expression primary_expression postfix_expression argument_expression_list unary_expression multiplicative_expression additive_expression comparison_expression expression logical_neg_expression
%type <decla> declarator declarator_list parameter_list parameter_declaration
%type <st> type_name
%type <ao> assignment_operator
%type <c> expression_statement declaration declaration_list compound_statement statement_list statement jump_statement iteration_statement selection_statement function_definition
%start program
%union {
  char *string_c;
  int n;
  double d;
  struct expression *s;
  enum simple_type st;
  enum assignment_op ao;
  struct code_container *c;
  struct declaration_list *decla;
}
%%

primary_expression
: IDENTIFIER
{
    $$ = new expression($1, global_hash_table);
    free($1); $1 = nullptr;
}
| CONSTANTI    
{
    $$ = new expression($1, global_hash_table);
}
| CONSTANTD
{
    $$ = new expression($1, global_hash_table);
}
| '(' expression ')'
{
    $$ = $2;
}
| IDENTIFIER '(' ')' //appel de fonction
{
    cout << "call" << endl;
    $$ = new expression($1, global_hash_table);
}
| IDENTIFIER '(' argument_expression_list ')' //appel de fonction
;

postfix_expression
: primary_expression
{
    $$ = $1;
}
| IDENTIFIER INC_OP
// simplification de : postfix_expression INC_OP
{
    struct expression *e1 = new expression($1, global_hash_table);
    struct expression *e2;
    switch (e1->getT()) {
    case _INT:
        e2 = new expression(1, global_hash_table);
        break;

    case _DOUBLE:
        e2 = new expression(1.0, global_hash_table);
        break;

    default:
        e2 = new expression(_ERROR, -1, global_hash_table);
        break;
    }
    struct expression *e3 = (*e1 + *e2);
    $$ = (*e3 = $1);
    $$->setVar(e1->getVar()); //Expression result is the identifier value before the unary operator
    delete e1; e1 = nullptr; delete e2, e2 = nullptr; delete e3; e3 = nullptr;
    free($1); $1 = nullptr;
}
| IDENTIFIER DEC_OP
// simplification de postfix_expression DEC_OP
{
    struct expression *e1 = new expression($1, global_hash_table);
    struct expression *e2;
    switch (e1->getT()) {
    case _INT:
        e2 = new expression(1, global_hash_table);
        break;

    case _DOUBLE:
        e2 = new expression(1.0, global_hash_table);
        break;

    default:
        e2 = new expression(_ERROR, -1, global_hash_table);
        break;
    }
    struct expression *e3 = (*e1 - *e2);
    $$ = (*e3 = $1);
    $$->setVar(e1->getVar()); //Expression result is the identifier value before the unary operator
    delete e1; e1 = nullptr; delete e2, e2 = nullptr; delete e3; e3 = nullptr;
    free($1); $1 = nullptr;
}
;

argument_expression_list
: expression
| argument_expression_list ',' expression
;

unary_expression
: postfix_expression
{
    $$ = $1;
}
| INC_OP IDENTIFIER
// simplification de : INC_OP unary_expression
{
    struct expression *e1 = new expression($2, global_hash_table);
    struct expression *e2;
    switch (e1->getT()) {
    case _INT:
        e2 = new expression(1, global_hash_table);
        break;

    case _DOUBLE:
        e2 = new expression(1.0, global_hash_table);
        break;

    default:
        e2 = new expression(_ERROR, -1, global_hash_table);
        break;
    }
    struct expression *e3 = (*e1 + *e2);
    $$ = (*e3 = $2);
    delete e1; e1 = nullptr; delete e2, e2 = nullptr; delete e3; e3 = nullptr;
    free($2); $2 = nullptr;
}
| DEC_OP IDENTIFIER
// simplification de : DEC_OP unary_expression
{
    struct expression *e1 = new expression($2, global_hash_table);
    struct expression *e2;
    switch (e1->getT()) {
    case _INT:
        e2 = new expression(1, global_hash_table);
        break;

    case _DOUBLE:
        e2 = new expression(1.0, global_hash_table);
        break;

    default:
        e2 = new expression(_ERROR, -1, global_hash_table);
        break;
    }
    struct expression *e3 = (*e1 - *e2);
    $$ = (*e3 = $2);
    delete e1; e1 = nullptr; delete e2, e2 = nullptr; delete e3; e3 = nullptr;
    free($2); $2 = nullptr;
}
| '-' unary_expression
{
    struct expression *e1;
    switch ($2->getT()) {
    case _INT:
        e1 = new expression(0, global_hash_table);
        break;

    case _DOUBLE:
        e1 = new expression(0.0, global_hash_table);
        break;

    default:
        e1 = new expression(_ERROR, -1, global_hash_table);
        cerr << "expression type is not valid" << endl;
        break;
    }
    $$ = (*e1 - *$2);
    delete e1; e1 = nullptr; delete $2; $2 = nullptr;
}
;

multiplicative_expression
: unary_expression
{
    $$ = $1;
}
| multiplicative_expression '*' unary_expression
{
    $$ = *$1 * *$3;
    delete $1; $1 = nullptr; delete $3; $3 = nullptr;
}
| multiplicative_expression '/' unary_expression
{
    $$ = *$1 / *$3;
    delete $1; $1 = nullptr; delete $3; $3 = nullptr;
}
| multiplicative_expression REM unary_expression
{
    $$ = *$1 % *$3;
    delete $1; $1 = nullptr; delete $3; $3 = nullptr;
}
;

additive_expression
: multiplicative_expression
{
    $$ = $1;
}
| additive_expression '+' multiplicative_expression
{
    $$ = *$1 + *$3;
    delete $1; $1 = nullptr; delete $3; $3 = nullptr;
}
| additive_expression '-' multiplicative_expression
{
    $$ = *$1 - *$3;
    delete $1; $1 = nullptr; delete $3; $3 = nullptr;
}
;

shift_expression
: additive_expression
{
    $$ = $1;
}
| shift_expression SHL additive_expression
{
    $$ = *$1 << *$3;
    delete $1; $1 = nullptr; delete $3; $3 = nullptr;
}
| shift_expression SHR additive_expression
{
    $$ = *$1 >> *$3;
    delete $1; $1 = nullptr; delete $3; $3 = nullptr;
}
;

comparison_expression
: shift_expression
{
    $$ = $1;
}
| comparison_expression '<' shift_expression
{
    $$ = *$1 < *$3;
    delete $1; $1 = nullptr; delete $3; $3 = nullptr;
}
| comparison_expression '>' shift_expression
{
    $$ = *$1 > *$3;
    delete $1; $1 = nullptr; delete $3; $3 = nullptr;
}
| comparison_expression LE_OP shift_expression
{
    $$ = *$1 <= *$3;
    delete $1; $1 = nullptr; delete $3; $3 = nullptr;
}
| comparison_expression GE_OP shift_expression
{
    $$ = *$1 >= *$3;
    delete $1; $1 = nullptr; delete $3; $3 = nullptr;
}
| comparison_expression EQ_OP shift_expression
{
    $$ = *$1 == *$3;
    delete $1; $1 = nullptr; delete $3; $3 = nullptr;
}
| comparison_expression NE_OP shift_expression
{
    $$ = *$1 != *$3;
    delete $1; $1 = nullptr; delete $3; $3 = nullptr;
}
;

logical_neg_expression
: comparison_expression
{
    $$ = $1;
}
| '!' comparison_expression
{
    $$ = !*$2;
    delete $2; $2 = nullptr;
}
;

logical_and_expression
: logical_neg_expression
{
    $$ = $1;
}
| logical_and_expression AND comparison_expression
{
    $$ = *$1 && *$3;
    delete $1; $1 = nullptr; delete $3; $3 = nullptr;
}
;

logical_or_expression
: logical_and_expression
{
    $$ = $1;
}
| logical_or_expression OR logical_and_expression
{
    $$ = *$1 || *$3;
    delete $1; $1 = nullptr; delete $3; $3 = nullptr;
}
;

conditional_expression
: logical_or_expression
{
    $$ = $1;
}
;

expression
: IDENTIFIER assignment_operator conditional_expression
// simplification de : unary_expression assignment_operator conditional_expression
{
    switch ($2) {

        case _EQ_ASSIGN:
            $$ = (*$3 = $1);
            break;

        case _ADD_ASSIGN:
            $$ = (*$3 += $1);
            break;

        case _SUB_ASSIGN:
            $$ = (*$3 -= $1);
            break;

        case _MUL_ASSIGN:
            $$ = (*$3 *= $1);
            break;

        case _DIV_ASSIGN:
            $$ = (*$3 /= $1);
            break;

        case _REM_ASSIGN:
            $$ = (*$3 %= $1);
            break;

        case _SHL_ASSIGN:
            $$ = (*$3 <<= $1);
            break;

        case _SHR_ASSIGN:
            $$ = (*$3 >>= $1);
            break;

        default:
            $$ = new expression(_ERROR, -1, global_hash_table);
            cerr << "Wrong assignment type" << endl;
            break;
    }

    delete $3; $3 = nullptr; free($1); $1 = nullptr;
}
| conditional_expression
{
    $$ = $1;
}
;

assignment_operator
: '='
{
    $$ = _EQ_ASSIGN;
}
| MUL_ASSIGN
{
    $$ = _MUL_ASSIGN;
}
| DIV_ASSIGN
{
    $$ = _DIV_ASSIGN;
}
| REM_ASSIGN
{
    $$ = _REM_ASSIGN;
}
| SHL_ASSIGN
{
    $$ = _SHL_ASSIGN;
}
| SHR_ASSIGN
{
    $$ = _SHR_ASSIGN;
}
| ADD_ASSIGN
{
    $$ = _ADD_ASSIGN;
}
| SUB_ASSIGN
{
    $$ = _SUB_ASSIGN;
}
;

declaration
: type_name declarator_list ';'
{
    $$ = new code_container();
    struct identifier id;
    BOOST_FOREACH(identifier id_old, $2->idList) {
        switch ($1) {
            case _INT:
                $$->code << "%" << id_old.name << " = alloca i32\n";
                break;
            case _DOUBLE:
                $$->code << "%" << id_old.name << " = alloca double\n";
                break;
            default:
                cout << "ERROR\n";
                break;
        }

        id.t = $1;
        id.name = id_old.name;
        id.symbolType = _VAR;
        global_hash_table[id_old.name] = id;
    }

    delete $2;
    $2 = nullptr;
}
;

declarator_list
: declarator
{
    $$ = $1;
}
| declarator_list ',' declarator
{
    $$ = $1;
    $$->idList.insert($$->idList.end(), $3->idList.begin(), $3->idList.end());

    delete $3; $3 = nullptr;
}
;

type_name
: VOID
{
    $$ = _VOID;
}
| INT
{
    $$ = _INT;
}
| DOUBLE
{
    $$ = _DOUBLE;
}
;

declarator
: IDENTIFIER
{
    $$ = new declaration_list();

    struct identifier id;
    id.name = $1;

    $$->idList.push_back(id);

    free($1); $1 = nullptr;
}
| '(' declarator ')'
/*| declarator '(' parameter_list ')'
{
    cout << $3->code.str() << endl;
    $$ = new declarator();
}
| declarator '(' ')'
{
    cout << "NO PARAMETER" << endl;
}*/
;

parameter_list
: parameter_declaration
{
    $$ = $1;
}
| parameter_list ',' parameter_declaration
{
    $$ = $1;
    $$->code << ", " << $3->code.str();
    $$->idList.insert($$->idList.end(), $3->idList.begin(), $3->idList.end());

    delete $3;
    $3 = nullptr;
}
;

parameter_declaration
: type_name IDENTIFIER
// simplification de type_name declarator
{
    int var = new_var();
    $$ = new declaration_list();
    switch ($1) {
        case _INT:
            $$->code << "i32 %x" << var;
            break;
        case _DOUBLE:
            $$->code << "double %x" << var;
            break;
        default:
            cout << "ERROR" << endl;
            break;
    }

    struct identifier id;
    id.t = $1;
    id.name = $2;
    id.register_no = var;
    id.symbolType = _VAR;
    $$->idList.push_back(id);

    global_hash_table[$2] = id; //add variable to the global hash table variable
    free($2); $2 = nullptr;
}
;

statement
: compound_statement
{
    $$ = $1;
}
| expression_statement
{
    $$ = $1;
}
| selection_statement
{
    $$ = $1;
}
| iteration_statement
{
    $$ = $1;
}
| jump_statement
{
    $$ = $1;
}
;

compound_statement
: '{' '}'
{
    $$ = new code_container();

}
| '{' statement_list '}'
{
    $$ = new code_container();

    $$->code << $2->code.str();

    delete $2;
    $2 = nullptr;
}
| '{' declaration_list statement_list '}'
{
    $$ = new code_container();

    $$->code << $2->code.str() << $3->code.str();

    delete $2;
    $2 = nullptr;
    delete $3;
    $3 = nullptr;
}
| '{' declaration_list '}'
{
    $$ = new code_container();

    $$->code << $2->code.str();

    delete $2;
    $2 = nullptr;
}
;

declaration_list
: declaration
{
    $$ = $1;
}
| declaration_list declaration
{
    $$ = $1;
    $$->code << $2->code.str();

    delete $2;
    $2 = nullptr;
}
;

statement_list
: statement
{
    $$ = $1;
}
| statement_list statement
{
    $$ = $1;
    $$->code << $2->code.str();

    delete $2;
    $2 = nullptr;
}
;

expression_statement
: ';'
{
    $$ = new code_container();
}
| expression ';'
{
    $$ = new code_container();
    $$->code << $1->code.str();

    delete $1;
    $1 = nullptr;
}
;

selection_statement
: IF '(' expression ')' statement
{
    $$ = if_then_else(*$3, *$5);
    delete $3; $3 = nullptr; delete $5; $5 = nullptr;
}
| IF '(' expression ')' statement ELSE statement
{
    $$ = if_then_else(*$3, *$5, *$7);
    delete $3; $3 = nullptr; delete $5; $5 = nullptr; delete $7; $7 = nullptr;
}
| FOR '(' expression ';' expression ';' expression ')' statement
{
    $$ = for_then(*$3, *$5, *$7, *$9);
    delete $3; $3 = nullptr; delete $5; $5 = nullptr;
    delete $7; $7 = nullptr; delete $9; $9 = nullptr;

}
| FOR '(' expression ';' expression ';'            ')' statement
{
    $$ = for_then(*$3, *$5, nullptr, *$8);
    delete $3; $3 = nullptr; delete $5; $5 = nullptr; delete $8; $8 = nullptr;
}
| FOR '(' expression ';'            ';' expression ')' statement
{
    $$ = for_then(*$3, nullptr, *$6, *$8);
    delete $3; $3 = nullptr; delete $6; $6 = nullptr; delete $8; $8 = nullptr;
}
| FOR '(' expression ';'            ';'            ')' statement
{
    $$ = for_then(*$3, nullptr, nullptr, *$7);
    delete $3; $3 = nullptr; delete $7; $7 = nullptr;
}
| FOR '('            ';' expression ';' expression ')' statement
{
    $$ = for_then(nullptr, *$4, *$6, *$8);
    delete $4; $4 = nullptr; delete $6; $6 = nullptr; delete $8; $8 = nullptr;
}
| FOR '('            ';' expression ';'            ')' statement
{
    $$ = for_then(nullptr, *$4, nullptr, *$7);
    delete $4; $4 = nullptr; delete $7; $7 = nullptr;
}
| FOR '('            ';'            ';' expression ')' statement
{
    $$ = for_then(nullptr, nullptr, *$5, *$7);
    delete $5; $5 = nullptr; delete $7; $7 = nullptr;
}
| FOR '('            ';'            ';'            ')' statement
{
    $$ = for_then(nullptr, nullptr, nullptr, *$6);
    delete $6; $6 = nullptr;
}
;

iteration_statement
: WHILE '(' expression ')' statement
{
    $$ = while_then(*$3, *$5);
    delete $3; $3 = nullptr; delete $5; $5 = nullptr;
}
| DO  statement  WHILE '(' expression ')'
{
    $$ = do_while(*$2, *$5);
    delete $2; $2 = nullptr; delete $5; $5 = nullptr;
}
;

jump_statement
: RETURN ';'
{
    $$ = new code_container();
    $$->code << "ret void\n";
}
| RETURN expression ';'
{
    $$ = new code_container();
    $$->code << $2->code.str() << "ret ";
    switch ($2->getT()) {
        case _INT:
            $$->code << "i32 %x" << $2->getVar();
            break;
        case _DOUBLE:
            $$->code << "double %x" << $2->getVar();
            break;
        default:
            cout << "ERROR\n";
            break;
    }
    $$->code << "\n";

    delete $2;
    $2 = nullptr;
}
;

program
: external_declaration
| program external_declaration
;

external_declaration
: function_definition
| declaration
;

function_definition
/* : type_name declarator compound_statement */
: type_name IDENTIFIER '(' parameter_list ')' compound_statement
{
    code << "define ";
    switch ($1) {
        case _INT:
            code << "i32 ";
            break;
        case _DOUBLE:
            code << "double ";
            break;
        case _VOID:
            code << "void ";
            break;
        default:
            cout << "ERROR 1\n";
            break;
    }
    code << "@" << $2 << " (" << $4->code.str() << ")\n" << "{\n";
    add_identifier($4->idList, code);
    code << $6->code.str() << "}\n" << "\n";

    // add function name to the hash table
    struct identifier id;
    id.t = $1;
    id.name = $2;
    id.symbolType = _FUNCTION;
    BOOST_FOREACH(identifier id_old, $4->idList) {
        id.paramTypes.push_back(id_old.t);
    }
    global_hash_table[$2] = id;

    delete $4; $4 = nullptr; delete $6; $6 = nullptr; free($2); $2 = nullptr;
}
| type_name IDENTIFIER '(' ')' compound_statement
{
    code << "define ";
    switch ($1) {
        case _INT:
            code << "i32 ";
            break;
        case _DOUBLE:
            code << "double ";
            break;
        case _VOID:
            code << "void ";
            break;
        default:
            cout << "ERROR 1\n";
            break;
    }
    code << "@" << $2 << " ()\n"  << "{\n";
    code << $5->code.str() << "}\n" << "\n";

    // add function name to the hash table
    struct identifier id;
    id.t = $1;
    id.name = $2;
    id.symbolType = _FUNCTION;
    global_hash_table[$2] = id;

    delete $5; $5 = nullptr; free($2); $2 = nullptr;
}
;

%%
#include <stdio.h>
#include <string.h>

extern int yylineno;
extern int yydebug;

char *file_name = nullptr;
extern char yytext[];
extern int column;
extern int yylineno;
extern FILE *yyin;


int yyerror (const char *s) {

    cerr << file_name << ":" << yylineno << ":" << column << ": " << s << endl;
    return 0;
}


int main (int argc, char *argv[]) {

    FILE *input = nullptr;
    if (argc==2) {
        input = fopen(argv[1], "r");
	file_name = strdup (argv[1]);
	if (input) {
	    yyin = input;
	}
	else {
            cerr << argv[0] << ": Could not open " << argv[1] << endl;
            return EXIT_FAILURE;
	}
    }
    else {
        cerr << argv[0] << ": error: no input file" << endl;
        return EXIT_FAILURE;
    }

    yyparse();
    cout << code.str();

    free(file_name);
    fclose(input);

    BOOST_FOREACH(map_boost::value_type i, global_hash_table) {
        if (i.second.symbolType == _VAR)
            cout<<"VAR : " << i.first<<" :"<<i.second.t<<','<<i.second.name<<endl;
        else if (i.second.symbolType == _FUNCTION) {
            cout<<"FUN : " << i.first<<" :"<<i.second.t<<','<<i.second.name<<endl;
            BOOST_FOREACH(enum simple_type st, i.second.paramTypes)
                cout << "  - " << st << endl;
        }
    }

    return 0;
}
