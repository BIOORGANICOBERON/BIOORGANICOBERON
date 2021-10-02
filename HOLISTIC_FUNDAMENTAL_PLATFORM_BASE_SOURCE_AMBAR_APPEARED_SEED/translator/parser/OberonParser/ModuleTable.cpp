/*
Oberon2 compiler for x86
Copyright (c) 2012 Evgeniy Grigorievitch Philippov
Distributed under the terms of GNU General Public License, v.3 or later
*/
#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>
#include "../common/common.h"
#include "ModuleTable.h"
#include "Parser.h"
#include "../generator/CodeGenerator.h"

//for wstrlen
//#include <synce.h>

//for wsprintf
//#include <widec.h>

//int wstrlen(const wchar_t* p){
//	int len = 0;
//	while(*p++)len++;
//	return len;
//}




//ModuleTable::ModuleTable(Errors *errors_)

//void ModuleTable::Err(const wchar_t* msg)

//Module* ModuleTable::NewModule(Parser::ModuleRecord &moduleAST)


// search the name in all open scopes and return its object node
//Module* ModuleTable::Find (wchar_t* name)


