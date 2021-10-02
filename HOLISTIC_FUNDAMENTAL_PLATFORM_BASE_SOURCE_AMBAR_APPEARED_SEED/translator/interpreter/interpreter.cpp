#include <stdio.h>
#include <wchar.h>

#include "../common/common.h"
#include "../parser/SymbolTable.h"
#include "../parser/Parser.h"
#include "../parser/Scanner.h"
#include <sys/timeb.h>
#include "../parser/ModuleTable.h"
#include "../generator/CodeGenerator.h"

int run(ModTab::ModuleTable *modules, wchar_t* fileName){
    wprintf(L"Attempting to run %ls...\n",fileName);
    FileInputStream *fis = new FileInputStream(fileName);
    BufferedInputStream *unpreprocessedIS=new BufferedInputStream(fis);
    BufferedInputStream* bis=new BufferedInputStream(
    			new PreprocessingInputStream(unpreprocessedIS));
	Scanner *scanner = new Scanner(bis); abortIfNull(scanner);
	Parser *parser = new Parser(scanner, modules->errors); abortIfNull(parser);
	int errorsCount2=parser->errors->count;
	parser->modtab=modules;
	//parser->addParserListener(ParserListener)
	parser->tab = new SymbolTable(parser); abortIfNull(parser->tab);
	parser->gen = new CodeGenerator(modules);	abortIfNull(parser->gen);
	wprintf(L"Parsing %ls...\n",fileName);
	parser->Parse();
	bis->close();
	wprintf(L"Adding to modcache...");
	modules->NewModule(parser, *parser->modulePtr);
	//Obj* mo = parser->tab->NewObj("MODULE",OKscope,new TypeMODULE(),parser);
	wprintf(L"Added.\n");
	int errorsCount=parser->errors->count;
	if (errorsCount == 0) {
		wprintf(L"Parsed successfully! Interpreting %ls.\n", parser->modulePtr->moduleName);
		errorsCount = parser->gen->InterpretModule(parser, parser->modulePtr, *(parser->tab), modules);
	}else{
		wprintf(L"Parsing of %ls failed: %d errors.\n",fileName, errorsCount);
	}

/*	coco_string_delete(fileName);
	delete parser->gen;
	delete parser->tab;
	delete parser;
	delete scanner;
*/
	return errorsCount;
}

