%{
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif //_GNU_SOURCE
#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "expression_symbols.hpp"
#include <boost/unordered_map.hpp>
#include <string>

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

//Hash map
typedef boost::unordered_map<std::string, int> map_boost;

//utility functions
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

%}

%define parse.error verbose

%token <string> IDENTIFIER
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
%type <s> conditional_expression logical_or_expression logical_and_expression shift_expression primary_expression postfix_expression argument_expression_list unary_expression multiplicative_expression additive_expression comparison_expression expression
%start program
%union {
  char *string;
  int n;
  double d;
  struct expression *s;
}
%%

conditional_expression
: logical_or_expression
{
    $$ = $1;
}
;

logical_or_expression
: logical_and_expression
{
    $$ = $1;
}
| logical_or_expression OR logical_and_expression
;

logical_and_expression
: comparison_expression
{
    $$ = $1;
}
| logical_and_expression AND comparison_expression
;


shift_expression
: additive_expression
{
    //cout << $1->code.str();
    //delete $1; //TODO : remove it
}
| shift_expression SHL additive_expression
| shift_expression SHR additive_expression
;

primary_expression
: IDENTIFIER
{
    $$ = new expression(_INT, new_var());
}
| CONSTANTI    
{
    $$ = new expression(_INT, new_var());
    $$->code << "%x" << $$->getVar() << " = add i32 0, " << $1 << "\n";
}
| CONSTANTD
{
    $$ = new expression(_DOUBLE, new_var());
    char *nb_double = double_to_hex_str($1);
    $$->code << "%x" << $$->getVar() << " = fadd double 0x000000000000000, " << nb_double << "\n";
    free(nb_double);
}
| '(' expression ')'
{
    $$ = $2;
}
| IDENTIFIER '(' ')' //appel de fonction
| IDENTIFIER '(' argument_expression_list ')' //appel de fonction
;

postfix_expression
: primary_expression
{
    $$ = $1;
}
| postfix_expression INC_OP
{
    if ($1->getT() == _INT) {
        $$ = new expression(_INT, new_var());
        $$->code << $1->code.str() << "%x" << $$->getVar() << " = add i32 %x" << $1->getVar() << ", 1\n";
    }
    else if ($1->getT() == _DOUBLE) {
        $$ = new expression(_INT, new_var());
        char *nb_double = double_to_hex_str(1.0);
        $$->code << $1->code.str() << "%x" << $$->getVar() << " = fadd double %x" << $1->getVar() << ", " << nb_double << "\n";
        free(nb_double);
    }
    delete $1;
    $1 = NULL;
}
| postfix_expression DEC_OP
{
    if ($1->getT() == _INT) {
        $$ = new expression(_INT, new_var());
        $$->code << $1->code.str() << "%x" << $$->getVar() << " = sub i32 %x" << $1->getVar() << ", 1\n";
    }
    else if ($1->getT() == _DOUBLE) {
        $$ = new expression(_INT, new_var());
        char *nb_double = double_to_hex_str(1.0);
        $$->code << $1->code.str() << "%x" << $$->getVar() << " = fsub double %x" << $1->getVar() << ", " << nb_double << "\n";
        free(nb_double);
    }
    delete $1;
    $1 = NULL;
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
| INC_OP unary_expression
{
    if ($2->getT() == _INT) {
        $$ = new expression(_INT, new_var());
        $$->code << $2->code.str() << "%x" << $$->getVar() << " = add i32 %x" << $2->getVar() << ", 1\n";
    }
    else if ($2->getT() == _DOUBLE) {
        $$ = new expression(_INT, new_var());
        char *nb_double = double_to_hex_str(1.0);
        $$->code << $2->code.str() << "%x" << $$->getVar() << " = fadd double %x" << $2->getVar() << ", " << nb_double << "\n";
        free(nb_double);
    }
    delete $2;
    $2 = NULL;
}
| DEC_OP unary_expression
{
    if ($2->getT() == _INT) {
        $$ = new expression(_INT, new_var());
        $$->code << $2->code.str() << "%x" << $$->getVar() << " = sub i32 %x" << $2->getVar() << ", 1\n";
    }
    else if ($2->getT() == _DOUBLE) {
        $$ = new expression(_INT, new_var());
        char *nb_double = double_to_hex_str(1.0);
        $$->code << $2->code.str() << "%x" << $$->getVar() << " = fsub double %x" << $2->getVar() << ", " << nb_double << "\n";
        free(nb_double);
    }
    delete $2;
    $2 = NULL;
}
| '-' unary_expression
{
    if ($2->getT() == _INT) {
        $$ = new expression(_INT, new_var());
        $$->code << $2->code.str() << "%x" << $$->getVar() << " = sub i32 0, %x" << $2->getVar() << "\n";
    }
    else if ($2->getT() == _DOUBLE) {
        $$ = new expression(_INT, new_var());
        $$->code << $2->code.str() << "%x" << $$->getVar() << " = fsub double 0x000000000000000, %x" << $2->getVar() << "\n";
    }
    delete $2;
    $2 = NULL;
}
;

/*unary_operator
: '-'
;*/

multiplicative_expression
: unary_expression
{
    $$ = $1;
}
| multiplicative_expression '*' unary_expression
{
    if ($1->getT() == _INT) {
        if ($3->getT() == _INT) {
            $$ = new expression(_INT, new_var());
            $$->code << $1->code.str() << $3->code.str() << "%x" << $$->getVar() << " = mul i32 %x" << $1->getVar() << ", %x" << $3->getVar() << "\n";
        }
        else if ($3->getT() == _DOUBLE) {
            int conversion = new_var();
            $$ = new expression(_DOUBLE, new_var());
            $$->code << $1->code.str() << $3->code.str() << "%x" << conversion << " = sitofp i32 %x" << $1->getVar() << " to double\n";
            $$->code << "%x" << $$->getVar() << " = fmul double %x" << $3->getVar() << ", %x" << conversion << "\n";
        }
    }
    else if($1->getT() == _DOUBLE) {
        if ($3->getT() == _INT) {
            int conversion = new_var();
            $$ = new expression(_DOUBLE, new_var());
            $$->code << $1->code.str() << $3->code.str() << "%x" << conversion << " = sitofp i32 %x" << $3->getVar() << " to double\n";
            $$->code << "%x" << $$->getVar() << " = fmul double %x" << $1->getVar() << ", %x" << conversion << "\n";
        }
        else if ($3->getT() == _DOUBLE) {
            $$ = new expression(_DOUBLE, new_var());
            $$->code << $1->code.str() << $3->code.str() << "%x" << $$->getVar() << " = fadd double %x" << $1->getVar() << ", %x" << $3->getVar() << "\n";
            //asprintf(&$$->old, "%s%s%%x%d = fmul double %%x%d, %%x%d\n", $1->code, $3->code, $$->getVar(), $1->getVar(), $3->getVar());
        }
    }
    delete $1;
    $1 = NULL;
    delete $3;
    $3 = NULL;
}
| multiplicative_expression '/' unary_expression
{
    if ($1->getT() == _INT) {
        if ($3->getT() == _INT) {
            $$ = new expression(_INT, new_var());
            $$->code << $1->code.str() << $3->code.str() << "%x" << $$->getVar() << " = sdiv i32 %x" << $1->getVar() << ", %x" << $3->getVar() << "\n";
        }
        else if ($3->getT() == _DOUBLE) {
            int conversion = new_var();
            $$ = new expression(_DOUBLE, new_var());
            $$->code << $1->code.str() << $3->code.str() << "%x" << conversion << " = sitofp i32 %x" << $1->getVar() << " to double\n";
            $$->code << "%x" << $$->getVar() << " = fdiv double %x" << $3->getVar() << ", %x" << conversion << "\n";
        }
    }
    else if($1->getT() == _DOUBLE) {
        if ($3->getT() == _INT) {
            int conversion = new_var();
            $$ = new expression(_DOUBLE, new_var());
            $$->code << $1->code.str() << $3->code.str() << "%x" << conversion << " = sitofp i32 %x" << $3->getVar() << " to double\n";
            $$->code << "%x" << $$->getVar() << " = fdiv double %x" << $1->getVar() << ", %x" << conversion << "\n";
        }
        else if ($3->getT() == _DOUBLE) {
            $$ = new expression(_DOUBLE, new_var());
            $$->code << $1->code.str() << $3->code.str() << "%x" << $$->getVar() << " = fadd double %x" << $1->getVar() << ", %x" << $3->getVar() << "\n";
            //asprintf(&$$->old, "%s%s%%x%d = fdiv double %%x%d, %%x%d\n", $1->code, $3->code, $$->getVar(), $1->getVar(), $3->getVar());
        }
    }
    delete $1;
    $1 = NULL;
    delete $3;
    $3 = NULL;
}
| multiplicative_expression REM unary_expression
{
    if ($1->getT() == _INT) {
        if ($3->getT() == _INT) {
            $$ = new expression(_INT, new_var());
            $$->code << $1->code.str() << $3->code.str() << "%x" << $$->getVar() << " = srem i32 %x" << $1->getVar() << ", %x" << $3->getVar() << "\n";
        }
        else if ($3->getT() == _DOUBLE) {
            int conversion = new_var();
            $$ = new expression(_DOUBLE, new_var());
            $$->code << $1->code.str() << $3->code.str() << "%x" << conversion << " = sitofp i32 %x" << $1->getVar() << " to double\n";
            $$->code << "%x" << $$->getVar() << " = frem double %x" << $3->getVar() << ", %x" << conversion << "\n";
        }
    }
    else if($1->getT() == _DOUBLE) {
        if ($3->getT() == _INT) {
            int conversion = new_var();
            $$ = new expression(_DOUBLE, new_var());
            $$->code << $1->code.str() << $3->code.str() << "%x" << conversion << " = sitofp i32 %x" << $3->getVar() << " to double\n";
            $$->code << "%x" << $$->getVar() << " = frem double %x" << $1->getVar() << ", %x" << conversion << "\n";
        }
        else if ($3->getT() == _DOUBLE) {
            $$ = new expression(_DOUBLE, new_var());
            $$->code << $1->code.str() << $3->code.str() << "%x" << $$->getVar() << " = fadd double %x" << $1->getVar() << ", %x" << $3->getVar() << "\n";
            //asprintf(&$$->old, "%s%s%%x%d = frem double %%x%d, %%x%d\n", $1->code, $3->code, $$->getVar(), $1->getVar(), $3->getVar());
        }
    }
    delete $1;
    $1 = NULL;
    delete $3;
    $3 = NULL;
}
;

additive_expression
: multiplicative_expression
{
    $$ = $1;
}
| additive_expression '+' multiplicative_expression
{
    if ($1->getT() == _INT) {
        if ($3->getT() == _INT) {
            $$ = new expression(_INT, new_var());
            $$->code << $1->code.str() << $3->code.str() << "%x" << $$->getVar() << " = add i32 %x" << $1->getVar() << ", %x" << $3->getVar() << "\n";
        }
        else if ($3->getT() == _DOUBLE) {
            int conversion = new_var();
            $$ = new expression(_DOUBLE, new_var());
            $$->code << $1->code.str() << $3->code.str() << "%x" << conversion << " = sitofp i32 %x" << $1->getVar() << " to double\n";
            $$->code << "%x" << $$->getVar() << " = fadd double %x" << $3->getVar() << ", %x" << conversion << "\n";
        }
    }
    else if($1->getT() == _DOUBLE) {
        if ($3->getT() == _INT) {
            int conversion = new_var();
            $$ = new expression(_DOUBLE, new_var());
            $$->code << $1->code.str() << $3->code.str() << "%x" << conversion << " = sitofp i32 %x" << $3->getVar() << " to double\n";
            $$->code << "%x" << $$->getVar() << " = fadd double %x" << $1->getVar() << ", %x" << conversion << "\n";
        }
        else if ($3->getT() == _DOUBLE) {
            $$ = new expression(_DOUBLE, new_var());
            $$->code << $1->code.str() << $3->code.str() << "%x" << $$->getVar() << " = fadd double %x" << $1->getVar() << ", %x" << $3->getVar() << "\n";
            //asprintf(&$$->old, "%s%s%%x%d = fadd double %%x%d, %%x%d\n", $1->code, $3->code, $$->getVar(), $1->getVar(), $3->getVar());
        }
    }
    delete $1;
    $1 = NULL;
    delete $3;
    $3 = NULL;
}
| additive_expression '-' multiplicative_expression
{
    if ($1->getT() == _INT) {
        if ($3->getT() == _INT) {
            $$ = new expression(_INT, new_var());
            $$->code << $1->code.str() << $3->code.str() << "%x" << $$->getVar() << " = sub i32 %x" << $1->getVar() << ", %x" << $3->getVar() << "\n";
        }
        else if ($3->getT() == _DOUBLE) {
            int conversion = new_var();
            $$ = new expression(_DOUBLE, new_var());
            $$->code << $1->code.str() << $3->code.str() << "%x" << conversion << " = sitofp i32 %x" << $1->getVar() << " to double\n";
            $$->code << "%x" << $$->getVar() << " = fsub double %x" << $3->getVar() << ", %x" << conversion << "\n";
        }
    }
    else if($1->getT() == _DOUBLE) {
        if ($3->getT() == _INT) {
            int conversion = new_var();
            $$ = new expression(_DOUBLE, new_var());
            $$->code << $1->code.str() << $3->code.str() << "%x" << conversion << " = sitofp i32 %x" << $3->getVar() << " to double\n";
            $$->code << "%x" << $$->getVar() << " = fsub double %x" << $1->getVar() << ", %x" << conversion << "\n";
        }
        else if ($3->getT() == _DOUBLE) {
            $$ = new expression(_DOUBLE, new_var());
            $$->code << $1->code.str() << $3->code.str() << "%x" << $$->getVar() << " = fadd double %x" << $1->getVar() << ", %x" << $3->getVar() << "\n";
            //asprintf(&$$->old, "%s%s%%x%d = fsub double %%x%d, %%x%d\n", $1->code, $3->code, $$->getVar(), $1->getVar(), $3->getVar());
        }
    }
    delete $1;
    $1 = NULL;
    delete $3;
    $3 = NULL;
}
;

comparison_expression
: shift_expression
{
    $$ = $1;
}
| comparison_expression '<' shift_expression
| comparison_expression '>' shift_expression
| comparison_expression LE_OP shift_expression
| comparison_expression GE_OP shift_expression
| comparison_expression EQ_OP shift_expression
| comparison_expression NE_OP shift_expression
;

expression
: unary_expression assignment_operator conditional_expression
| conditional_expression
{
    $$ = $1;
}
;

assignment_operator
: '='
| MUL_ASSIGN
| DIV_ASSIGN
| REM_ASSIGN
| SHL_ASSIGN
| SHR_ASSIGN
| ADD_ASSIGN
| SUB_ASSIGN
;

declaration
: type_name declarator_list ';'
;

declarator_list
: declarator
| declarator_list ',' declarator
;

type_name
: VOID
| INT
| DOUBLE
;

declarator
: IDENTIFIER
| '(' declarator ')'
| declarator '(' parameter_list ')'
| declarator '(' ')'
;

parameter_list
: parameter_declaration
| parameter_list ',' parameter_declaration
;

parameter_declaration
: type_name declarator
;

statement
: compound_statement
| expression_statement
| selection_statement
| iteration_statement
| jump_statement
;

compound_statement
: '{' '}'
| '{' statement_list '}'
| '{' declaration_list statement_list '}'
| '{' declaration_list '}'
;

declaration_list
: declaration
| declaration_list declaration
;

statement_list
: statement
| statement_list statement
;

expression_statement
: ';'
| expression ';'
{
    cout << $1->code.str();
    delete $1;
}
;

selection_statement
: IF '(' expression ')' statement
| IF '(' expression ')' statement ELSE statement
| FOR '(' expression ';' expression ';' expression ')' statement
| FOR '(' expression ';' expression ';'            ')' statement
| FOR '(' expression ';'            ';' expression ')' statement
| FOR '(' expression ';'            ';'            ')' statement
| FOR '('            ';' expression ';' expression ')' statement
| FOR '('            ';' expression ';'            ')' statement
| FOR '('            ';'            ';' expression ')' statement
| FOR '('            ';'            ';'            ')' statement
;

iteration_statement
: WHILE '(' expression ')' statement
| DO  statement  WHILE '(' expression ')'
;

jump_statement
: RETURN ';'
| RETURN expression ';'
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
: type_name declarator compound_statement
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
    fflush (stdout);
    fprintf (stderr, "%s:%d:%d: %s\n", file_name, yylineno, column, s);
    return 0;
}


int main (int argc, char *argv[]) {

    map_boost x;
    x["one"] = 1;
    x["two"] = 2;
    x["three"] = 3;

    cout << x.at("one") << endl;
    cout << x.size() << endl;
    cout << x.max_size() << endl;
    assert(x.at("one") == 1);
    assert(x.find("missing") == x.end());

    FILE *input = NULL;
    if (argc==2) {
	input = fopen (argv[1], "r");
	file_name = strdup (argv[1]);
	if (input) {
	    yyin = input;
	}
	else {
	  fprintf (stderr, "%s: Could not open %s\n", *argv, argv[1]);
	    return 1;
	}
    }
    else {
	fprintf (stderr, "%s: error: no input file\n", *argv);
	return 1;
    }

    yyparse();

    free(file_name);

    return 0;
}
