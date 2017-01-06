%{
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif //_GNU_SOURCE

#define DEBUG //yack ?

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>

#include "expression.hpp"
#include "statement.hpp"

using namespace std;
namespace po = boost::program_options;

// headers definition to permit compilation
extern int yylineno;
int yyerror(const char *s);
string file_name;
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
bool debug;
ofstream output;

//Hash map
map_boost global_hash_table;

// Create global reference table and add the global map
map_list ref_tab;


stringstream code;

%}

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
    $$ = new expression($1, ref_tab);
    free($1); $1 = nullptr;
}
| CONSTANTI    
{
    $$ = new expression($1, ref_tab);
}
| CONSTANTD
{
    $$ = new expression($1, ref_tab);
}
| '(' expression ')'
{
    $$ = $2;
}
| IDENTIFIER '(' ')' //appel de fonction
{
    $$ = new expression($1, nullptr, ref_tab);
    free($1); $1 = nullptr;
}
| IDENTIFIER '(' argument_expression_list ')' //appel de fonction
{
    $$ = new expression($1, *$3, ref_tab);
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
    $$ = incr_postfix($1, ref_tab);
    free($1); $1 = nullptr;
}
| IDENTIFIER DEC_OP
// simplification de postfix_expression DEC_OP
{
    $$ = decr_postfix($1, ref_tab);
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
    $$ = incr_prefix($2, ref_tab);
    free($2); $2 = nullptr;
}
| DEC_OP IDENTIFIER
// simplification de : DEC_OP unary_expression
{
    $$ = decr_prefix($2, ref_tab);
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
| logical_and_expression AND logical_neg_expression
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
            $$ = new expression(_ERROR, -1, ref_tab);
            error_funct(_ERROR_COMPIL, "Unexpected symbol");
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
    $$ = local_declaration($1, *$2, ref_tab);
    delete $2; $2 = nullptr;
}
;

global_declaration
: type_name declarator_list ';'
{
    $$ = global_declaration($1, *$2, ref_tab);
    delete $2; $2 = nullptr;
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
    $$ = new declaration_list($1, $2, nullptr, ref_tab);
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
    delete $2; $2 = nullptr;
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
    $$->code << "  ret void\n";
}
| RETURN expression ';'
{
    $$ = return_statement(*$2);
    delete $2; $2 = nullptr;
}
;

program_entry
: program
{
    struct code_container *cc = declare_q5_used_functions(ref_tab);

    if (!has_error) {
        if (debug) {
            cout << cc->code.str() << "\n";
            cout << $1->code.str();
        }
        else {
            output << cc->code.str() << "\n";
            output << $1->code.str();
        }
    }

    delete cc; cc = nullptr; delete $1; $1 = nullptr;
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
    $$ = declare_funct($1, $2, *$4, ref_tab);
    free($2); $2 = nullptr; delete $4; $4 = nullptr;
}
| type_name IDENTIFIER '(' ')' ';'
{
    $$ = declare_funct($1, $2, ref_tab);
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
: type_name IDENTIFIER '(' parameter_list ')'
{
    $<c>$ = define_funct($1, $2, ref_tab);

    // complete function definition
    struct identifier& id = ref_tab.front().at($2);
    BOOST_FOREACH(identifier id_old, $4->idList) {
        id.paramTypes.push_back(id_old.t);
    }

    ref_tab.push_front(map_boost());

    // add id to hash table
    map_boost& hash_table = ref_tab.front();
    BOOST_FOREACH(identifier id_old, $4->idList) {
        hash_table[id_old.hash_name] = id_old;
    }

}
compound_statement
{
    $$ = $<c>6;
    $$->code << $4->code.str() << ")\n" << "{\n";
    add_identifier($4->idList, $$->code);
    $$->code << $7->code.str();

    if ($1 != _VOID && $7->has_return == false)
        error_funct(_ERROR_COMPIL, "control reaches end of non-void function");
    if ($1 == _VOID && $7->has_return == false)
        $$->code << "  ret void\n";

    $$->code << "}\n\n";

    ref_tab.pop_front();
    delete $4; $4 = nullptr; delete $7; $7 = nullptr; free($2); $2 = nullptr;
}
| type_name IDENTIFIER '(' ')'
{
    $<c>$ = define_funct($1, $2, ref_tab);
    ref_tab.push_front(map_boost());
}
compound_statement
{
    $$ = $<c>5;
    $$->code << ")\n" << "{\n" << $6->code.str();

    if ($1 != _VOID && $6->has_return == false)
        error_funct(_ERROR_COMPIL, "control reaches end of non-void function");
    if ($1 == _VOID && $6->has_return == false)
        $$->code << "  ret void\n";

    $$->code << "}\n\n";

    ref_tab.pop_front();
    delete $6; $6 = nullptr; free($2); $2 = nullptr;
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

    po::options_description
    desc("Packer usage : ./packer [options] input-file\nAllowed options ");
    desc.add_options()
    ("help", "produce help message")
    ("input-file", po::value<string>()->required(), "input file path")
    ("output-file,o", po::value<string>(), "output file path (default is file_name.ll)")
    ("debug,d", po::value<bool>(), "show debug informations : program output and hash table");
    po::variables_map vm; //Parameters container
    po::positional_options_description p; //Used to indicate input file without --input-file
    p.add("input-file", -1);

    try {
        //Effectively parse the command line
        po::store(po::command_line_parser(argc, argv).
                  options(desc).positional(p).allow_unregistered().run(), vm);

        if (vm.count("help")) {
            cerr << desc << endl;
            return EXIT_SUCCESS;
        }

        //Check parsing errors (required parameters, ...)
        po::notify(vm);
    }
    catch (exception& e) {
        cerr << "Error : " << e.what() << endl;
        return EXIT_FAILURE;
    }

    // Open input file
    FILE *input = nullptr; //C file are used because yacc don't accept C++ ones
    file_name = vm["input-file"].as<string>().c_str();
    input = fopen(file_name.c_str(), "r");
    if (input) {
        yyin = input;
    }
    else {
        cerr << argv[0] << ": Could not open " << file_name.c_str() << endl;
        return EXIT_FAILURE;
    }

    // Open output file
    if (vm.count("output-file")) {
        output.open(vm["output-file"].as<string>());
    }
    else {
        string output_file = vm["input-file"].as<string>();
        output_file.erase(output_file.size()-1).append("ll");
        output.open(output_file);
    }

    // setup debug flag
    (vm.count("debug")) ? debug = true : debug = false;

    // setup map_list
    ref_tab.push_front(map_boost());
    setup_p5(ref_tab.front());


    if (yyparse() == 1)
        return EXIT_FAILURE;

    /*BOOST_FOREACH(map_boost::value_type i, ref_tab) {
        if (i.second.from_q5 == false && !i.second.used) {
            if (i.second.symbolType == _LOCAL_VAR || i.second.symbolType == _GLOBAL_VAR)
                error_funct(_WARNING_COMPIL, "unused variable ", i.first);
            else if (i.second.symbolType == _FUNCTION && i.first != "main")
                // We can use == comparison because i.first is a string (cf utilityFunction header)
                error_funct(_WARNING_COMPIL, "unused function ", i.first);
        }
    }*/

    if (debug) {

        BOOST_FOREACH(list<map_boost>::value_type hash, ref_tab) {

            cout << "Intern Symbol table at the end of parser execution" << endl;
            BOOST_FOREACH(map_boost::value_type i, hash) {
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

            cout << endl << "p5 Symbol table at the end of parser execution" << endl;
            BOOST_FOREACH(map_boost::value_type i, hash) {
                if (i.second.from_q5 == true) {
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
        }
    }


    fclose(input);
    output.close();
    if (has_error)
        return EXIT_FAILURE;
    else
        return EXIT_SUCCESS;
}
