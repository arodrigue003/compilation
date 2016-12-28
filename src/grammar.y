%{
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif //_GNU_SOURCE

#define DEBUG //yack ?

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>

#include "expression.hpp"
#include "statement.hpp"

using namespace std;

// headers definition to permit compilation
extern int yylineno;
int yyerror(const char *s);
char *file_name = nullptr;
extern int column;
extern int yylineno;

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
bool has_error = false;

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
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%type <s> conditional_expression logical_or_expression logical_and_expression shift_expression primary_expression postfix_expression unary_expression multiplicative_expression additive_expression comparison_expression expression logical_neg_expression
%type <decla> declarator declarator_list parameter_list parameter_declaration
%type <st> type_name
%type <ao> assignment_operator
%type <c> expression_statement declaration declaration_list compound_statement statement_list statement jump_statement iteration_statement selection_statement function_definition
%type <c> global_declaration program external_declaration function_declaration
%type <ael> argument_expression_list
%type <tl> type_list
%start program_entry
%union {
  char *string_c;
  int n;
  double d;
  struct expression *s;
  enum simple_type st;
  enum assignment_op ao;
  struct code_container *c;
  struct declaration_list *decla;
  struct arg_expr_list *ael;
  struct type_list *tl;
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
    $$ = new expression($1, nullptr, global_hash_table);
    free($1); $1 = nullptr;
}
| IDENTIFIER '(' argument_expression_list ')' //appel de fonction
{
    $$ = new expression($1, *$3, global_hash_table);
    free($1); $1 = nullptr; delete $3; $3 = nullptr;
}
;

postfix_expression
: primary_expression
{
    $$ = $1;
}
| IDENTIFIER INC_OP
// simplification de : postfix_expression INC_OP
{
    $$ = incr_postfix($1, global_hash_table);
    free($1); $1 = nullptr;
}
| IDENTIFIER DEC_OP
// simplification de postfix_expression DEC_OP
{
    $$ = decr_postfix($1, global_hash_table);
    free($1); $1 = nullptr;
}
;

argument_expression_list
: expression
{
    $$ = new arg_expr_list();
    $$->codeV.push_back($1);
}
| argument_expression_list ',' expression
{
    $$ = $1;
    $$->codeV.push_back($3);
}
;

unary_expression
: postfix_expression
{
    $$ = $1;
}
| INC_OP IDENTIFIER
// simplification de : INC_OP unary_expression
{
    $$ = incr_prefix($2, global_hash_table);
    free($2); $2 = nullptr;
}
| DEC_OP IDENTIFIER
// simplification de : DEC_OP unary_expression
{
    $$ = decr_prefix($2, global_hash_table);
    free($2); $2 = nullptr;
}
| '-' unary_expression
{
    $$ = opposite(*$2);
    delete $2; $2 = nullptr;
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
        id.name = "%" + id_old.name;
        id.symbolType = _LOCAL_VAR;
        global_hash_table[id_old.name] = id;
    }

    delete $2;
    $2 = nullptr;
}
;

global_declaration
: type_name declarator_list ';'
{
    $$ = new code_container();
    struct identifier id;
    BOOST_FOREACH(identifier id_old, $2->idList) {
        switch ($1) {
            case _INT:
                $$->code << "@" << id_old.name << " = common global i32 0\n";
                break;
            case _DOUBLE:
                $$->code << "@" << id_old.name << " = common global double 0x000000000000000\n";
                break;
            default:
                cout << "ERROR\n";
                break;
        }

        id.t = $1;
        id.name = "@" + id_old.name;
        id.symbolType = _GLOBAL_VAR;
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

    delete $3; $3 = nullptr;
}
;

parameter_declaration
: type_name IDENTIFIER
// simplification de type_name declarator
{
    $$ = new declaration_list($1, $2, global_hash_table);
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
    $$ = $2;
}
| '{' declaration_list statement_list '}'
{
    $$ = $2;
    $$->code << $3->code.str();
    $$->has_return = $3->has_return;
    delete $3; $3 = nullptr;
}
| '{' declaration_list '}'
{
    $$ = $2;
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
    if ($$->has_return == false) {
        $$->code << $2->code.str();
        $$->has_return = $2->has_return;
    }
    delete $2; $2 = nullptr;
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
    delete $1; $1 = nullptr;
}
;

selection_statement
: IF '(' expression ')' statement %prec LOWER_THAN_ELSE
{
    $$ = if_then_else(*$3, *$5);
    delete $3; $3 = nullptr; delete $5; $5 = nullptr;
}
| IF '(' expression ')' statement ELSE statement
{;
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
    $$->has_return = true;
    $$->code << "ret void\n";
}
| RETURN expression ';'
{
    $$ = new code_container();
    $$->has_return = true;
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

program_entry
: program
{
    struct code_container *cc = declare_q5_used_functions(global_hash_table);
    cout << cc->code.str() << "\n";
    delete cc; cc = nullptr;
    cout << $1->code.str();
    delete $1; $1 = nullptr;
}
;

program
: external_declaration
{
    $$ = $1;
}
| program external_declaration
{
    $$ = $1;
    $1->code << $2->code.str();
    delete $2; $2 = nullptr;
}
;

external_declaration
: function_definition
{
    $$ = $1;
}
| global_declaration
{
    $$ = $1;
}
| function_declaration
{
    $$ = $1;
}
;

function_declaration
: type_name IDENTIFIER '(' type_list ')' ';'
{
    $$ = declare_funct($1, $2, *$4, global_hash_table);
    free($2); $2 = nullptr; delete $4; $4 = nullptr;
}
| type_name IDENTIFIER '(' ')' ';'
{
    $$ = declare_funct($1, $2, global_hash_table);
    free($2); $2 = nullptr;
}
;

type_list
: type_name
{
    $$ = new type_list();
    $$->list.push_back($1);
}
| type_list ',' type_name
{
    $$->list.push_back($3);
}
;

function_definition
/* : type_name declarator compound_statement */
: type_name IDENTIFIER '(' parameter_list ')' compound_statement
{
    $$ = new code_container();
    $$->code << "define ";
    switch ($1) {
        case _INT:
            $$->code << "i32 ";
            break;
        case _DOUBLE:
            $$->code << "double ";
            break;
        case _VOID:
            $$->code << "void ";
            break;
        default:
            cout << "ERROR 1\n";
            break;
    }
    $$->code << "@" << $2 << " (" << $4->code.str() << ")\n" << "{\n";
    add_identifier($4->idList, $$->code);
    $$->code << $6->code.str();

    if ($1 != _VOID && $6->has_return == false)
        error_funct(_ERROR_COMPIL, "control reaches end of non-void function");
    if ($1 == _VOID && $6->has_return == false)
        $$->code << "  ret void\n";

    $$->code << "}\n\n";

    // add function name to the hash table
    struct identifier id;
    id.t = $1;
    string temp = $2;
    id.name = "@" + temp;
    id.symbolType = _FUNCTION;
    BOOST_FOREACH(identifier id_old, $4->idList) {
        id.paramTypes.push_back(id_old.t);
    }
    global_hash_table[$2] = id;        

    delete $4; $4 = nullptr; delete $6; $6 = nullptr; free($2); $2 = nullptr;
}
| type_name IDENTIFIER '(' ')' compound_statement
{
    $$ = new code_container();
    $$->code << "define ";
    switch ($1) {
        case _INT:
            $$->code << "i32 ";
            break;
        case _DOUBLE:
            $$->code << "double ";
            break;
        case _VOID:
            $$->code << "void ";
            break;
        default:
            cout << "ERROR 1\n";
            break;
    }
    $$->code << "@" << $2 << " ()\n"  << "{\n";
    $$->code << $5->code.str();

    if ($1 != _VOID && $5->has_return == false)
        error_funct(_ERROR_COMPIL, "control reaches end of non-void function");
    if ($1 == _VOID && $5->has_return == false)
        $$->code << "  ret void\n";

    $$->code << "}\n\n";

    // add function name to the hash table
    struct identifier id;
    id.t = $1;
    string temp = $2;
    id.name = "@" + temp;
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


extern char yytext[];
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

    setup_p5(global_hash_table);

    yyparse();


    BOOST_FOREACH(map_boost::value_type i, global_hash_table) {
        if (i.second.from_q5 == false && !i.second.used) {
            if (i.second.symbolType == _LOCAL_VAR || i.second.symbolType == _GLOBAL_VAR)
                error_funct(_WARNING_COMPIL, "unused variable ", i.first);
            else if (i.second.symbolType == _FUNCTION && i.first != "main")
                // We can use == comparison because i.first is a string (cf utilityFunction header)
                error_funct(_WARNING_COMPIL, "unused function ", i.first);
        }
    }


    BOOST_FOREACH(map_boost::value_type i, global_hash_table) {
        if (i.second.from_q5 == false) {
            if (i.second.symbolType == _LOCAL_VAR)
                cout<<"LVAR : " << i.first<<" :"<<i.second.t<<','<<i.second.name<<','<<i.second.used<<endl;
            else if (i.second.symbolType == _GLOBAL_VAR)
                cout<<"GVAR : " << i.first<<" :"<<i.second.t<<','<<i.second.name<<','<<i.second.used<<endl;
            else if (i.second.symbolType == _FUNCTION) {
                cout<<"FUNC : " << i.first<<" :"<<i.second.t<<','<<i.second.name<<','<<i.second.used<<endl;
                BOOST_FOREACH(enum simple_type st, i.second.paramTypes)
                    cout << "  - " << st << endl;
            }
        }
    }


    free(file_name);
    fclose(input);
    if (has_error)
        return EXIT_FAILURE;
    else
        return EXIT_SUCCESS;
}
