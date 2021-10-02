#include "CodeGenerator.h"
#include "../parser/ModuleTable.h"
#include "../parser/Parser.h"
#include "../parser/Types.h"
#include <assert.h>
#include <stdio.h>
#include <wchar.h>

/*
void CodeGenerator::Disassemble(Parser* parser) {
	int maxPc = pc;
	wprintf(L"TOP PC: %3d\n", pc);
	wprintf(L"DISASSEMBLING:\n");
	pc = 0;
	while (pc < maxPc) {
		int code = Next();
		wprintf(L"%3x:\t%2x\t%ls\t", pc-1, code, (code==-1?L"-1":opcode[code]));
		if (code == LOAD || code == LOADG || code == CONST || code == STO || code == STOG ||
			code == CALL || code == ENTER || code == JMP   || code == FJMP)
				wprintf(L"%d\n", Next2());
		else
			if (code == ADD  || code == SUB || code == MUL || code == DIV || code == NEG ||
				code == EQU  || code == LSS || code == GTR || code == RET || code == LEAVE ||
				code == READ || code == WRITE)
					wprintf(L"\n");
			else
				wprintf(L"\n");
	}
}
*/
//using namespace Oberon;

/*void CodeGenerator::WriteObjFile(Oberon::Parser::ModuleRecord &moduleAST){
	int len=wstrlen(moduleAST.moduleName);
	wchar_t *objFileName=new wchar_t[4+len+4+1];
	swprintf(objFileName, L"Obj/%ls.Obj", moduleAST.moduleName);
	wmkdir(L"Obj");
	FILE* objFile=wfopen(objFileName, "wb");
	fwrite(objFile, code, pc);
	fclose(objFile);
}*/


//void CodeGenerator::IMPORT(wchar_t* moduleName)

//void CodeGenerator::InterpretImport(Parser::ImportListRecord* ip)
//void CodeGenerator::InterpretModule(Parser::ModuleRecord *moduleASTPtr, SymbolTable &tab)

//void CodeGenerator::InterpretModuleDeclSeq(Parser::DeclSeqRecord &declSeq, SymbolTable &tab)
//void CodeGenerator::InterpretModuleInit(Parser::ModuleRecord *modAST, SymbolTable &tab)

//void CodeGenerator::InterpretStmtSeq(Parser::StatementSeqRecord& stmtSeq, SymbolTable &tab)

//CodeGenerator::CodeGenerator(ModuleTable *modules)


/*	//----- code generation methods -----

void CodeGenerator::Emit (char op) {
		code[pc++] = op;
		if(pc==codeSize){
			int newCodeSize=codeSize+32*1024;
			char *newCode=new char[newCodeSize];
			if(newCode==0){
				wprintf(L"Not enough memory.\n");
				exit(2);
			}
			char*cp=code;
			char*ncp=newCode;
			int cnt=codeSize;
			while(cnt-- > 0){
				*ncp++=*cp++;
			}
			char*oldCode=code;
			code=newCode;
			codeSize=newCodeSize;
			delete[] oldCode;
		}
	}

void CodeGenerator::Emit (char op, short val) {
		Emit(op); Emit((char)(val>>8)); Emit((char)val);
	}

void CodeGenerator::Patch (int adr, int val) {
		code[adr] = (char)(val>>8); code[adr+1] = (char)val;
	}

//void CodeGenerator::InterpretModule(Parser::ModuleRecord *moduleASTPtr, Oberon::SymbolTable &tab);
//void WriteObjFile(Oberon::ModuleRecord &moduleAST);
*/
/*
  //----- interpreter methods -----

	int Next () {
		return code[pc++];
	}

	int Next2 () {
		int x,y;
		x = (char)code[pc++]; y = code[pc++];
		return ((x << 8)&( ( (int)-1 ) ^ (int)0xff )) | y;
	}

	int Int (bool b) {
		if (b) return 1; else return 0;
	}

	void Push (int val) {
		stack[top++] = val;
	}

	int Pop() {
		return stack[--top];
	}

	int ReadInt(FILE* s) {
		int sign;
		char ch;
		do {fscanf(s, "%c", &ch);} while (!(ch >= '0' && ch <= '9' || ch == '-'));

		if (ch == '-') {sign = -1; fscanf(s, "%c", &ch);} else sign = 1;
		int n = 0;
		while (ch >= '0' && ch <= '9') {
			n = 10 * n + (ch - '0');
			if (fscanf(s, "%c", &ch) <= 0)
				break;
		}
		return n * sign;
	}
	void Interpret () {
		int val;
		pc = 0; stack[0] = 0; top = 1; bp = 0;
		wprintf(L"PC:=%d\n",pc);
		wprintf(L"INTERPRETING:\n");
		for (;;) {
			wprintf(L"PC:%3d; ",pc);
			if (pc == -1) {wprintf(L"NMI: EXIT"); return;}
			int nxt = Next();
			if (nxt == CONST)
				Push(Next2());
			else if (nxt == LOAD)
				Push(stack[bp+Next2()]);
			else if (nxt == LOADG)
				Push(globals[Next2()]);
			else if (nxt == STO)
				stack[bp+Next2()] = Pop();
			else if (nxt == STOG)
				globals[Next2()] = Pop();
			else if (nxt == ADD)
				Push(Pop()+Pop());
			else if (nxt == SUB)
				Push(-Pop()+Pop());
			else if (nxt == DIV)
				{val = Pop(); Push(Pop()/val);}
			else if (nxt == MUL)
				Push(Pop()*Pop());
			else if (nxt == NEG)
				Push(-Pop());
			else if (nxt == EQU)
				Push(Int(Pop()==Pop()));
			else if (nxt == LSS)
				Push(Int(Pop()>Pop()));
			else if (nxt == GTR)
				Push(Int(Pop()<Pop()));
			else if (nxt == JMP)
				{pc = Next2(); wprintf(L"JMP %d\n",pc);}
			else if (nxt == FJMP)
				{ val = Next2(); if (Pop()==0) pc = val;}
			else if (nxt == WRITE)
				printf("%d\n", Pop());
			else if (nxt == CALL)
				{Push(pc+2); pc = Next2();}
			else if (nxt == RET)
				{puts("RET");pc = Pop();}
			else if (nxt == ENTER)
				{Push(bp); bp = top; top = top + Next2();}
			else if (nxt == LEAVE)
				{top = bp; bp = Pop();}
			else {
				wprintf(L"NMI: illegal opcode\n");
				exit(1);
			}
		}
	}
*/

wchar_t* wstrconcat(const wchar_t* a, int alenchars, const wchar_t* b, const int blenchars){
	wchar_t *buf = (wchar_t*)malloc((alenchars+blenchars+1)*sizeof(wchar_t));
	abortIfNull(buf);
	int len = 0;
	while(*a){buf[len++]=*a++;}
	while(*b){buf[len++]=*b++;}
	buf[len]=(wchar_t)0;
	return buf;
}

int CodeGenerator::IMPORT(wchar_t* moduleName){
	ModTab::Module* m = modules->Find(moduleName);
	if (m==0){
		size_t len = mywstrlen(moduleName);
		wchar_t* fileName = (wchar_t*) malloc ((len+5)*sizeof(wchar_t));
		swprintf(fileName, len+5, L"%ls%ls", moduleName, L".Mod");
		return run(modules, fileName);
	}else{
		wprintf(L"Imported %ls from a modcache.\n",moduleName);///TODO analyse updated files
		return 0;
	}
}
