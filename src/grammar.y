%{
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif //_GNU_SOURCE
#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "expression_symbols.h"
#include <boost/unordered_map.hpp>

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
%type <s> primary_expression postfix_expression unary_expression multiplicative_expression additive_expression
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
;

logical_or_expression
: logical_and_expression
| logical_or_expression OR logical_and_expression
;

logical_and_expression
: comparison_expression
| logical_and_expression AND comparison_expression
;


shift_expression
: additive_expression
{
    printf("%s", $1->code);
}
| shift_expression SHL additive_expression
| shift_expression SHR additive_expression
;

primary_expression
: IDENTIFIER
| CONSTANTI    
{
    $$ = create_expression(_INT, new_var());
    asprintf(&$$->code, "%%x%d = add i32 0, %d\n", $$->var, $1);
}
| CONSTANTD
{
    $$ = create_expression(_DOUBLE, new_var());
    char *nb_double = double_to_hex_str($1);
    asprintf(&$$->code, "%%x%d = fadd double %s, 0x0000000000000000\n", $$->var, nb_double);
    free(nb_double);
}
| '(' expression ')'
| IDENTIFIER '(' ')'
| IDENTIFIER '(' argument_expression_list ')'
;

postfix_expression
: primary_expression
{
    $$ = $1;
}
| postfix_expression INC_OP
| postfix_expression DEC_OP
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
| DEC_OP unary_expression
| unary_operator unary_expression
;

unary_operator
: '-'
;

multiplicative_expression
: unary_expression
{
    $$ = $1;
}
| multiplicative_expression '*' unary_expression
{
    if ($1->t == _INT) {
        if ($3->t == _INT) {
            $$ = create_expression(_INT, new_var());
            asprintf(&$$->code, "%s%s%%x%d = add i32 %%x%d, %%x%d\n", $1->code, $3->code, $$->var, $1->var, $3->var);
        }
        else if ($3->t == _DOUBLE) {
            int conversion = new_var();
            $$ = create_expression(_INT, new_var());
            asprintf(&$$->code, "%s%s%%x%d = sitofp i32 %%x%d to double\n%%x%d = fadd double %%x%d, %%x%d\n", $1->code, $3->code, conversion, $1->var, $$->var, $3->var, conversion);
        }
    }
    else if($1->t == _DOUBLE) {
        if ($3->t == _INT) {
            int conversion = new_var();
            $$ = create_expression(_INT, new_var());
            asprintf(&$$->code, "%s%s%%x%d = sitofp i32 %%x%d to double\n%%x%d = fadd double %%x%d, %%x%d\n", $1->code, $3->code, conversion, $3->var, $$->var, $1->var, conversion);
        }
        else if ($3->t == _DOUBLE) {
            $$ = create_expression(_INT, new_var());
            asprintf(&$$->code, "%s%s%%x%d = fadd double %%x%d, %%x%d\n", $1->code, $3->code, $$->var, $1->var, $3->var);
        }
    }
    free($1);
    free($3);
}
| multiplicative_expression '/' unary_expression
| multiplicative_expression REM unary_expression
;

additive_expression
: multiplicative_expression
{
    $$ = $1;
}
| additive_expression '+' multiplicative_expression
| additive_expression '-' multiplicative_expression
;

comparison_expression
: shift_expression
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

    yyparse ();

    free(file_name);

    return 0;
}
