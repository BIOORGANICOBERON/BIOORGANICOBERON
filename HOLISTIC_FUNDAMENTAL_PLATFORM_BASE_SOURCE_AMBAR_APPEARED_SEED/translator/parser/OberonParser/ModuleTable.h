#if !defined(OBERON_SYMBOLTABLE_H)
#define OBERON_SYMBOLTABLE_H
/*
Oberon2 compiler for x86
Copyright (c) 2012 Evgeniy Grigorievitch Philippov
Distributed under the terms of GNU General Public License, v.3 or later
*/
#include "Scanner.h"
#include "Parser.h"
#include <assert.h>

class Errors;

namespace ModTab{
struct Module {  // object describing a declared name
public:
	//virtual AbstractBackend::TYPE getType()=0;
	Module(Parser* parser_):next(0),parser(parser_){}
	Parser::ModuleRecord *getAST(){return parser->modulePtr;}
	void addNext(Module* m){
		assert(m!=0);
		this->next=m;
	}
	Module *getnext(){return next;}
private:
	Module *next; // to next object in same scope //TODO reimplement as HashTable<wchar_t*,ModuleRecord*> name2moduleAST.
public:
	Parser* parser;
};

/*
struct InterpreterBackendModule: public Module{
	InterpreterBackendModule():memory(){}
	virtual ~InterpreterBackendModule(){}
	InterpreterBackendMemory getMemory(){return memory;}
	virtual AbstractBackend::TYPE getType(){return AbstractBackend::TYPE::interpreter;}
private:
	InterpreterBackendMemory memory;
};
*/

struct ModuleTable
{
	Errors *errors;
	Module *topScope;

	ModuleTable(Errors *errors_){
		errors = errors_;
		topScope=0;
	}
	void Err(const wchar_t* msg){
		errors->Error(0, 0, msg);
	}
	Module* NewModule(Parser *parser, Parser::ModuleRecord &moduleAST){
		wprintf(L"\nADDING TO MODCACHE: %ls\n", parser->modulePtr->moduleName);
		Module *p = topScope; Module *last = 0;
		while (p != 0) {
			if (coco_string_equal(p->getAST()->moduleName, moduleAST.moduleName)){
				const wchar_t *a=L"name declared twice: ";
				int len_a=mywstrlen(a);
				wchar_t* b = moduleAST.moduleName;
				int len_b = mywstrlen(b);
				wchar_t* msg = wstrconcat(a, len_a, b, len_b);
				Err(msg);
				free(msg);
				return 0;
			}
			last = p; p = p->getnext();
		}
		Module *obj = new Module(parser);
		abortIfNull(obj);
		bool last0= last == 0;
		if (last0){topScope=obj;}
		else last->addNext(obj);
		return obj;
	}
	Module* Find (wchar_t* name){
		Module *obj=topScope;
		while (obj != 0) {  // for all objects in this scope
			if (coco_string_equal(obj->getAST()->moduleName, name)) return obj;
			obj = obj->getnext();
		}
		return 0;
	}
};
}

#endif // !defined(OBERON_SYMBOLTABLE_H)
