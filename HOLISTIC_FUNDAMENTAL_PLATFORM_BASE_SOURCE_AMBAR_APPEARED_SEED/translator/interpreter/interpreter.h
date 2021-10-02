/*
 * interpreter.h
 *
 *  Created on: 09.12.2012
 *      Author: egp
 */

#ifndef INTERPRETER_H_
#define INTERPRETER_H_

#include "../parser/ModuleTable.h"
#include "wchar.h"

int run(ModTab::ModuleTable* modules, wchar_t* fileName);



#endif /* INTERPRETER_H_ */
