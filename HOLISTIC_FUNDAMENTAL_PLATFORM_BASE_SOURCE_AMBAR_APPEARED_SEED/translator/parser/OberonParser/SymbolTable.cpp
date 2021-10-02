/*
Oberon2 compiler for x86
Copyright (c) 2012 Evgeniy Grigorievitch Philippov
Distributed under the terms of GNU General Public License, v.3 or later
*/
#include "SymbolTable.h"
#include "Parser.h"



SymbolTable::SymbolTable(Parser *parser) {
	this->parser=parser;
	errors = parser->errors;
	topScope = NULL;
	curLevel = -1;
	//undefObj = new Obj();
	//undefObj->name  = coco_string_create("undef"); undefObj->type = 0; undefObj->kind = OKvar;
	//undefObj->adr = 0; undefObj->level = 0; undefObj->next = NULL;
}

void SymbolTable::Err(const wchar_t* msg) {
	errors->Error(0, 0, msg);
}


// open a new scope and make it the current scope (topScope)
Obj* SymbolTable::OpenScope () {
	Obj *scop = new Obj();
	scop->name = 0; scop->kind = OKscope;
	scop->locals = NULL; scop->nextAdr = 0;
	scop->next = topScope; topScope = scop;
	curLevel++;
	return scop;
}


// close the current scope //TODO destroy topScope->locals
void SymbolTable::CloseScope () {
	topScope = topScope->next; curLevel--;
}

// create a new object node in the current scope
Obj* SymbolTable::NewObj (wchar_t* name, object_kinds kind, TypeRecord* type, DataObject* data) {
	wprintf(L"\n  SYM NEW %ls: %ls::%ls\n", parser->modulePtr->moduleName, topScope->name, name);
	Obj *p, *last, *obj = new Obj();
	obj->name = coco_string_create(name); obj->kind = kind; obj->type = type;
	obj->data = data;
	obj->level = curLevel;
	p = topScope->locals; last = NULL;
	while (p != NULL) {
		if (coco_string_equal(p->name, name))
			Err(L"name declared twice");
		last = p; p = p->next;
	}
	if (last == NULL) topScope->locals = obj; else last->next = obj;
	if (kind == OKvar) obj->adr = topScope->nextAdr++;
	return obj;
}

// search the name in all open scopes and return its object node
Obj* SymbolTable::Find (wchar_t* name) {
	return Find0(name,false);
}

// search the name in all open scopes and return its object node
Obj* SymbolTable::Find0 (wchar_t* name, bool silent) {
	wprintf(L"[[[ SYM FIND %ls: %ls ]]]", parser->modulePtr->moduleName, name);
	Obj *obj, *scope;
	scope = topScope;
	while (scope != NULL) {  // for all open scopes
		obj = scope->locals;
		while (obj != NULL) {  // for all objects in this scope
			if (coco_string_equal(obj->name, name)) return obj;
			obj = obj->next;
		}
		scope = scope->next;
	}
	if(!silent){
		wchar_t str[100];
		coco_swprintf(str, 100, L"%ls is undeclared", name);
		Err(str);
	}
	return 0;//undefObj;
}

// search the name in all open scopes and return its object node
Obj* SymbolTable::FindSilent (wchar_t* name) {
	return Find0(name, true);
}

// search the name in a given scope and return its object node
Obj* SymbolTable::Find2 (Obj *scope_, wchar_t* name) {
	wprintf(L"\n    SYM FI2 %ls::%ls::%ls ", parser->modulePtr->moduleName, scope_->name, name);
	Obj *obj, *scope;
	scope = scope_;
	obj = scope->locals;
	while (obj != NULL) {  // for all objects in this scope
		if (coco_string_equal(obj->name, name)) return obj;
		obj = obj->next;
	}
	wchar_t str[100];
	coco_swprintf(str, 100, L"%ls is undeclared", name);
	Err(str);
	return 0;//undefObj;
}
