#ifndef OBERON_ZH_CODEGENERATOR_H
#define OBERON_ZH_CODEGENERATOR_H

/*
Oberon ZH compiler for x86
Copyright (c) 2012 Evgeniy Grigorievitch Philippov
Distributed under the terms of GNU General Public License, v.3 or later
*/

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>

#include "../common/common.h"
#include "../parser/SymbolTable.h"
#include "../parser/ModuleTable.h"
#include "../parser/Parser.h"

//run()
#include "../interpreter/interpreter.h"

class SymbolTable;


struct ModAliasRefDO: public DataObject{
	DataObjectKind getKind(){return ModAliasRefDOK;}
	wchar_t *modName;
};

class CodeGenerator {
	ModTab::ModuleTable *modules;
	void InterpretModuleDeclSeq(Parser::DeclSeqRecord &declSeq, SymbolTable &tab){
		//	struct DeclSeqRecord{
		//		DeclSeqConstTypeVarListRecord ctvList;
		//		DeclSeqProcDeclFwdDeclListRecord pfList;
		//	};
		//	struct DeclSeqConstTypeVarListRecord{
		//		bool specified;
		//		DeclSeqConstTypeVarListMandatoryRecord* constTypeVarListPtr; // undefined if specified==false
		//	};
			if(declSeq.ctvList.specified){
				assert(declSeq.ctvList.constTypeVarListPtr!=0);
				declSeq.ctvList.constTypeVarListPtr->interpret(*this, tab);
			}
			//	struct DeclSeqProcDeclFwdDeclListRecord{
			//		bool specified;
			//		DeclSeqProcDeclFwdDeclListMandatoryRecord* procDeclFwdDeclListPtr; // undefined if specified==false
			//	};
			if(declSeq.pfList.specified){
				assert(declSeq.pfList.procDeclFwdDeclListPtr!=0);
				declSeq.pfList.procDeclFwdDeclListPtr->interpret(*this, tab);
			}
		}
	void InterpretModuleInit(Parser*p, Parser::ModuleRecord *modAST, SymbolTable &tab){
		assert(modAST!=0);
	//	StatementSeqRecord* stmtSeq; //may be null if there's no MODULE Init section
		if(modAST->stmtSeq != 0){
			wprintf(L"\nBEGIN (* %ls *)", modAST->moduleName);
			InterpretStmtSeq(p, (modAST->stmtSeq), tab);
		}
		wprintf(L"END %ls.\n", modAST->moduleName);
	}
public:
	void InterpretStmtSeq(Parser*p, Parser::StatementSeqRecord* stmtSeq, SymbolTable &tab){
		//	StatementSeqRecord* stmtSeq; //may be null if there's no MODULE Init section
		//	struct StatementSeqRecord{
		//		StatementRecord *statementPtr;
		//		StatementSeqRecord* nullOrPtrToNextStatementSeq;
		//	};
			for(;;){
				if(stmtSeq==0)return;
				if (stmtSeq->statementPtr!=0){
					stmtSeq->statementPtr->interpret(p, tab);
				}
				stmtSeq=stmtSeq->nullOrPtrToNextStatementSeq;
			}
		}
public:
	int IMPORT(wchar_t* moduleName);
	int InterpretImport(Parser::ImportListRecord* ip, Parser* parser, Parser::ModuleRecord* moduleRecord, ModTab::ModuleTable * modtab){
		const Parser::ModuleImportEntryRecord& mr = ip->moduleImportEntry;
		int errCnt=0;
		if(mr.rhsPresent){
			const identRec & modAlias = mr.lhs;
			const identRec & modName = mr.rhs;
			TypeMODULE *tm = new TypeMODULE(); abortIfNull(tm);
//			Module* m = modtab->Find(modName);
//			if(m==0){wprintf(L"module %ls not found",modName);exit(1);}
			parser->tab->NewObj(modName, OKscope, tm, 0);
			ModAliasRefDO* DO = new ModAliasRefDO(); abortIfNull(DO);
			DO->modName = modName;
			parser->tab->NewObj(modAlias, OKscope, tm, DO);
			errCnt=IMPORT(modName);
		}else{
			const identRec & modName = mr.lhs;
			TypeMODULE *tm = new TypeMODULE(); abortIfNull(tm);
//			Module* m = modtab->Find(modName);
//			if(m==0){wprintf(L"module %ls not found",modName);exit(1);}

			parser->tab->NewObj(modName, OKscope, tm, 0);
			errCnt=IMPORT(modName);
		}
		return errCnt;
	}

public:
	CodeGenerator(ModTab::ModuleTable * modules_):modules(modules_){}

	~CodeGenerator(){}

	//----- code generation methods -----

//	void Emit (char op);
//
//	void Emit (char op, short val);
//
//	void Patch (int adr, int val);
//
//	//void Disassemble(Oberon::Parser* parser);

	int InterpretModule(Parser* p, Parser::ModuleRecord *moduleASTPtr, SymbolTable &tab, ModTab::ModuleTable * modtab){
		assert(moduleASTPtr!=0);
		Parser::ModuleRecord &moduleAST = *moduleASTPtr;
		tab.OpenScope();

		wprintf(L"\n(* %ls *)\n",moduleAST.moduleName);
		Parser::ImportListRecord* ip = moduleAST.importListPtr;
		while (ip!=0) {
			int errCnt = InterpretImport(ip, p, moduleASTPtr, modtab);
			if(errCnt !=0 ){
				return errCnt;
			}
			ip = ip->nullOrPtrToNextModuleImportEntriesList;
		}

		wprintf(L"\n(* %ls *)\n",moduleAST.moduleName);
		/*
		  "MODULE" Ident<r.moduleName> ";"
		  ( 	(. r.importListPtr=0; .)
		  |	(. r.importListPtr=new ImportListRecord(); abortIfNull(r.importListPtr); .)
			ImportList<*(r.importListPtr)>
		  )
		  DeclSeq<r.declSeq>
		  ["BEGIN" StatementSeq<r.stmtSeq>]
		  "END" ident "."
		 */
		InterpretModuleDeclSeq(moduleAST.declSeq, tab);

		if(!moduleAST.initialized){
			InterpretModuleInit(p, &moduleAST, tab);
			moduleAST.initialized=true;
		}
		//tab.CloseScope();
		//WriteObjFile(moduleAST);
		int errorsCount=p->errors->count;
		if (errorsCount == 0) {
			wprintf(L"Interpreted %ls successfully!\n",p->modulePtr->moduleName);
		}else{
			wprintf(L"Interpreting of %ls failed: %d errors.\n",p->modulePtr->moduleName, errorsCount);
		}
		return errorsCount;
	}
	//void WriteObjFile(Oberon::ModuleRecord *moduleAST);

  //----- interpreter methods -----

//	int Next () {
//		return code[pc++];
//	}
//
//	int Next2 () {
//		int x,y;
//		x = (char)code[pc++]; y = code[pc++];
//		return ((x << 8)&( ( (int)-1 ) ^ (int)0xff )) | y;
//	}
//
//	int Int (bool b) {
//		if (b) return 1; else return 0;
//	}
//
//	void Push (int val) {
//		stack[top++] = val;
//	}
//
//	int Pop() {
//		return stack[--top];
//	}
//
//	int ReadInt(FILE* s) {
//		int sign;
//		char ch;
//		do {fscanf(s, "%c", &ch);} while (!(ch >= '0' && ch <= '9' || ch == '-'));
//
//		if (ch == '-') {sign = -1; fscanf(s, "%c", &ch);} else sign = 1;
//		int n = 0;
//		while (ch >= '0' && ch <= '9') {
//			n = 10 * n + (ch - '0');
//			if (fscanf(s, "%c", &ch) <= 0)
//				break;
//		}
//		return n * sign;
//	}
//	void Interpret () {
//		int val;
//		pc = 0; stack[0] = 0; top = 1; bp = 0;
//		wprintf(L"PC:=%d\n",pc);
//		wprintf(L"INTERPRETING:\n");
//		for (;;) {
//			wprintf(L"PC:%3d; ",pc);
//			if (pc == -1) {wprintf(L"NMI: EXIT"); return;}
//			int nxt = Next();
//			if (nxt == CONST)
//				Push(Next2());
//			else if (nxt == LOAD)
//				Push(stack[bp+Next2()]);
//			else if (nxt == LOADG)
//				Push(globals[Next2()]);
//			else if (nxt == STO)
//				stack[bp+Next2()] = Pop();
//			else if (nxt == STOG)
//				globals[Next2()] = Pop();
//			else if (nxt == ADD)
//				Push(Pop()+Pop());
//			else if (nxt == SUB)
//				Push(-Pop()+Pop());
//			else if (nxt == DIV)
//				{val = Pop(); Push(Pop()/val);}
//			else if (nxt == MUL)
//				Push(Pop()*Pop());
//			else if (nxt == NEG)
//				Push(-Pop());
//			else if (nxt == EQU)
//				Push(Int(Pop()==Pop()));
//			else if (nxt == LSS)
//				Push(Int(Pop()>Pop()));
//			else if (nxt == GTR)
//				Push(Int(Pop()<Pop()));
//			else if (nxt == JMP)
//				{pc = Next2(); wprintf(L"JMP %d\n",pc);}
//			else if (nxt == FJMP)
//				{ val = Next2(); if (Pop()==0) pc = val;}
//			else if (nxt == WRITE)
//				printf("%d\n", Pop());
//			else if (nxt == CALL)
//				{Push(pc+2); pc = Next2();}
//			else if (nxt == RET)
//				{puts("RET");pc = Pop();}
//			else if (nxt == ENTER)
//				{Push(bp); bp = top; top = top + Next2();}
//			else if (nxt == LEAVE)
//				{top = bp; bp = Pop();}
//			else {
//				wprintf(L"NMI: illegal opcode\n");
//				exit(1);
//			}
//		}
//	}

};

#endif // !defined(OBERON_ZH_CODEGENERATOR_H)
