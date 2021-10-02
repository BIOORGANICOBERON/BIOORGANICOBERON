/*
Oberon2 compiler for x86
Copyright (c) 2012 Evgeniy Grigorievitch Philippov
Distributed under the terms of GNU General Public License, v.3 or later
*/

#include <stdio.h>
#include <wchar.h>

#include "common/common.h"
#include "parser/SymbolTable.h"
#include "parser/Parser.h"
#include "parser/Scanner.h"
#include <sys/timeb.h>
#include "parser/ModuleTable.h"
#include "parser/Types.h"

#include "interpreter/interpreter.h"

int main (const int argc, const char *argv[]) {
	if (argc == 2) {
		TypeQualidentVariants_static_init();
		wchar_t *fileName = coco_string_create(argv[1]);
		Errors *errors = new Errors(); abortIfNull(errors);
		ModTab::ModuleTable *modules = new ModTab::ModuleTable(errors);	abortIfNull(modules);
		int errorsCount=run(modules, fileName);
		if (errorsCount != 0) return 2;
		return 0;
	} else {
		wprintf(L"No source file name specified.\n");
		return 1;
	}
}
