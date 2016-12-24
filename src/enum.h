#ifndef ENUM_H
#define ENUM_H

enum simple_type
{
	_DOUBLE,
	_INT,
	_VOID,
	_BOOL,
	_ERROR
};

enum assignment_op
{
	_EQ_ASSIGN,
	_MUL_ASSIGN,
	_DIV_ASSIGN,
	_REM_ASSIGN,
	_SHL_ASSIGN,
	_SHR_ASSIGN,
	_ADD_ASSIGN,
	_SUB_ASSIGN
};

enum identifier_type
{
	_FUNCTION,
	_LOCAL_VAR,
	_GLOBAL_VAR
};

#endif // ENUM_H
