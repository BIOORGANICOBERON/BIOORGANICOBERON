#if !defined(ROD_SYMBOLTABLE_H)
#define ROD_SYMBOLTABLE_H
/*
Oberon2 compiler for x86
Copyright (c) 2012 Evgeniy Grigorievitch Philippov
Distributed under the terms of GNU General Public License, v.3 or later
*/
#include <wchar.h>

enum object_kinds {OKvar, OKconst, OKproc, OKscope, OKtype};

enum DataObjectKind {DeclSeqProcDOK, ModAliasRefDOK, DeclVarDOK, DeclConstDOK, DeclTypeDOK};

struct DataObject{
	virtual DataObjectKind getKind()=0;
	virtual ~DataObject(){}
};

class Obj;

class Obj {  // object describing a declared name
public:
	wchar_t* name;		// name of the object
	struct TypeRecord* type;		// type of the object (undef for proc)
	Obj	*next;		// to next object in same scope
	object_kinds kind;		// var, proc, scope
	int adr;		// address in memory or start of proc
	int level;		// nesting level; 0=global, 1=local
	Obj *locals;		// scopes: to locally declared objects
	int nextAdr;	// scopes: next free address in this scope
	DataObject* data; //declaration data
	void* valuePtr; //for interpreter backend
	size_t valueTypeSizeInBits; //for interpreter backend

	Obj() {
		name    = NULL;
		type    = NULL;
		next    = NULL;
		kind    = OKscope;
		adr     = 0;
		level   = 0;
		locals  = NULL;
		nextAdr = 0;
		data    = 0;
		valuePtr   = 0;
	}

	~Obj() {
		if (name != 0) {delete name; name=0;}
	}


};

class Errors;
class Parser;

class SymbolTable
{
public:

	int curLevel;	// nesting level of current scope
	//Obj *undefObj;	// object node for erroneous symbols
	Obj *topScope;	// topmost procedure scope

	Errors *errors;
	Parser *parser;

	SymbolTable(Parser *parser);
	void Err(const wchar_t* msg);

	// open a new scope and make it the current scope (topScope)
	Obj* OpenScope ();

	// close the current scope
	void CloseScope ();

	// create a new object node in the current scope
	Obj* NewObj (wchar_t* name, object_kinds kind, TypeRecord* type, DataObject* data);

	// search the name in all open scopes and return its object node
	Obj* Find (wchar_t* name);

	// search the name in all open scopes and return its object node
	Obj* FindSilent (wchar_t* name);

	// search the name in a given scopes and return its object node
	Obj* Find2 (Obj* scope, wchar_t* name);

private:
	// search the name in all open scopes and return its object node
	Obj* Find0 (wchar_t* name, bool silent);
};

#endif // !defined(ROD_SYMBOLTABLE_H)
