%{
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif //_GNU_SOURCE
#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "expression_symbols.hpp"
#include "enum.h"
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
vector<identifier> to_store;

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
%type <decla> declarator declarator_list
%type <st> type_name
%type <ao> assignment_operator
%type <c> parameter_list parameter_declaration function_definition
%type <c> expression_statement declaration declaration_list compound_statement statement_list statement jump_statement iteration_statement selection_statement
%start program
%union {
  char *string_c;
  int n;
  double d;
  struct expression *s;
  struct declarator *decla;
  enum simple_type st;
  enum assignment_op ao;
  struct code_container *c;
}
%%

primary_expression
: IDENTIFIER
{
    $$ = new expression($1, global_hash_table);
    free($1); $1 = NULL;
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
    delete e1; e1 = NULL; delete e2, e2 = NULL; delete e3; e3 = NULL;
    free($1); $1 = NULL;
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
    delete e1; e1 = NULL; delete e2, e2 = NULL; delete e3; e3 = NULL;
    free($1); $1 = NULL;
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
    delete e1; e1 = NULL; delete e2, e2 = NULL; delete e3; e3 = NULL;
    free($2); $2 = NULL;
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
    delete e1; e1 = NULL; delete e2, e2 = NULL; delete e3; e3 = NULL;
    free($2); $2 = NULL;
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
    delete e1; e1 = NULL; delete $2; $2 = NULL;
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
    delete $1; $1 = NULL; delete $3; $3 = NULL;
}
| multiplicative_expression '/' unary_expression
{
    $$ = *$1 / *$3;
    delete $1; $1 = NULL; delete $3; $3 = NULL;
}
| multiplicative_expression REM unary_expression
{
    $$ = *$1 % *$3;
    delete $1; $1 = NULL; delete $3; $3 = NULL;
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
    delete $1; $1 = NULL; delete $3; $3 = NULL;
}
| additive_expression '-' multiplicative_expression
{
    $$ = *$1 - *$3;
    delete $1; $1 = NULL; delete $3; $3 = NULL;
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
    delete $1; $1 = NULL; delete $3; $3 = NULL;
}
| shift_expression SHR additive_expression
{
    $$ = *$1 >> *$3;
    delete $1; $1 = NULL; delete $3; $3 = NULL;
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
    delete $1; $1 = NULL; delete $3; $3 = NULL;
}
| comparison_expression '>' shift_expression
{
    $$ = *$1 > *$3;
    delete $1; $1 = NULL; delete $3; $3 = NULL;
}
| comparison_expression LE_OP shift_expression
{
    $$ = *$1 <= *$3;
    delete $1; $1 = NULL; delete $3; $3 = NULL;
}
| comparison_expression GE_OP shift_expression
{
    $$ = *$1 >= *$3;
    delete $1; $1 = NULL; delete $3; $3 = NULL;
}
| comparison_expression EQ_OP shift_expression
{
    $$ = *$1 == *$3;
    delete $1; $1 = NULL; delete $3; $3 = NULL;
}
| comparison_expression NE_OP shift_expression
{
    $$ = *$1 != *$3;
    delete $1; $1 = NULL; delete $3; $3 = NULL;
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
    delete $2; $2 = NULL;
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
    delete $1; $1 = NULL; delete $3; $3 = NULL;
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
    delete $1; $1 = NULL; delete $3; $3 = NULL;
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

    delete $3; $3 = NULL; free($1); $1 = NULL;
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
    for (std::vector<string>::iterator it = $2->begin(); it != $2->end(); ++it){
        switch ($1) {
            case _INT:
                $$->code << "%" << *it << " = alloca i32\n";
                break;
            case _DOUBLE:
                $$->code << "%" << *it << " = alloca double\n";
                break;
            default:
                cout << "ERROR\n";
                break;
        }

        id.t = $1;
        id.name = *it;
        global_hash_table[*it] = id;
    }

    delete $2;
    $2 = NULL;
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
    $$->merge($3);

    delete $3;
    $3 = NULL;
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
    $$ = new declarator();
    $$->add($1);

    free($1);
    $1 = NULL;
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

    delete $3;
    $3 = NULL;
}
;

parameter_declaration
: type_name IDENTIFIER
// simplification de type_name declarator
{
    int var = new_var();
    $$ = new code_container();
    switch ($1) {
        case _INT:
            $$->code << "i32 %x" << var;
            break;
        case _DOUBLE:
            $$->code << "double %x" << var;
            break;
        default:
            cout << "ERROR\n";
            break;
    }
    struct identifier id;

    id.t = $1;
    id.name = $2;
    id.register_no = var;
    to_store.push_back(id);

    global_hash_table[$2] = id;
    free($2);
    $2 = NULL;
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
    $2 = NULL;
}
| '{' declaration_list statement_list '}'
{
    $$ = new code_container();

    $$->code << $2->code.str() << $3->code.str();

    delete $2;
    $2 = NULL;
    delete $3;
    $3 = NULL;
}
| '{' declaration_list '}'
{
    $$ = new code_container();

    $$->code << $2->code.str();

    delete $2;
    $2 = NULL;
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
    $2 = NULL;
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
    $2 = NULL;
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
    $1 = NULL;
}
;

selection_statement
: IF '(' expression ')' statement
{
    $$ = if_statement(*$3, *$5);
    delete $3; $3 = NULL; delete $5; $5 = NULL;
}
| IF '(' expression ')' statement ELSE statement
{
    $$ = if_else_statement(*$3, *$5, *$7);
    delete $3; $3 = NULL; delete $5; $5 = NULL; delete $7; $7 = NULL;
}
| FOR '(' expression ';' expression ';' expression ')' statement
{
    $$ = new code_container();
}
| FOR '(' expression ';' expression ';'            ')' statement
{
    $$ = new code_container();
}
| FOR '(' expression ';'            ';' expression ')' statement
{
$$ = new code_container();
}
| FOR '(' expression ';'            ';'            ')' statement
{
    $$ = new code_container();
}
| FOR '('            ';' expression ';' expression ')' statement
{
    $$ = new code_container();
}
| FOR '('            ';' expression ';'            ')' statement
{
    $$ = new code_container();
}
| FOR '('            ';'            ';' expression ')' statement
{
    $$ = new code_container();
}
| FOR '('            ';'            ';'            ')' statement
{
    $$ = new code_container();
}
;

iteration_statement
: WHILE '(' expression ')' statement
{
    $$ = new code_container();
}
| DO  statement  WHILE '(' expression ')'
{
    $$ = new code_container();
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
    $2 = NULL;
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
    add_identifier(to_store, code);
    code << $6->code.str() << "}\n" << "\n";

    // add function name to the hash table
    struct identifier id;
    id.t = $1;
    id.name = $2;
    id.register_no = -1;
    global_hash_table[$2] = id;

    delete $4;
    $4 = NULL;
    delete $6;
    $6 = NULL;
    free($2);
    $2 = NULL;
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
    add_identifier(to_store, code);;
    code << $5->code.str() << "}\n" << "\n";

    // add function name to the hash table
    struct identifier id;
    id.t = $1;
    id.name = $2;
    id.register_no = -1;
    global_hash_table[$2] = id;

    delete $5;
    $5 = NULL;
    free($2);
    $2 = NULL;
}
;

%%
#include <stdio.h>
#include <string.h>

extern int yylineno;
extern int yydebug;

char *file_name = NULL;
extern char yytext[];
extern int column;
extern int yylineno;
extern FILE *yyin;


int yyerror (const char *s) {

    cerr << file_name << ":" << yylineno << ":" << column << ": " << s << endl;
    return 0;
}


int main (int argc, char *argv[]) {

    FILE *input = NULL;
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
        std::cout<<i.first<<":"<<i.second.t<<','<<i.second.name<<"\n";
    }

    for (std::vector<identifier>::iterator it = to_store.begin(); it != to_store.end(); ++it){
        cout << (*it).register_no << ',' << (*it).t << ',' << (*it).name << endl;
    }

    return 0;
}
