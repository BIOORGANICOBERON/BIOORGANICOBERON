/*----------------------------------------------------------------------
Oberon suite
Copyright (c) 2012, 2014 Evgeniy Grigorievitch Philippov. All Rights Reserved.
-----------------------------------------------------------------------*/

#if !defined(Oberon_COCO_PARSER_H)
#define Oberon_COCO_PARSER_H

class CodeGenerator;

#include "stdio.h"
#include "wchar.h"
#include "assert.h"
#include "Types.h"
#include "../common/common.h"

#include "SymbolTable.h"
//#include "CodeGenerator.h"
#include "wchar.h"

namespace ModTab{class ModuleTable;}

struct ExprListRecord{
	ExprRecord expr;
	ExprListRecord *nullOrCommaExprList;
};

enum LiteralNumberTypeEnum {literal_int, literal_real};

struct literalNumberRecord{
	LiteralNumberTypeEnum literal_type;
	wchar_t* tokenString;
	Value* calculate_numberValue_from_literal();
};

#include "Scanner.h"

/* namespace Oberon {
 */
class Errors {
public:
	int count;			// number of errors detected

	Errors();
	void SynErr(int line, int col, int n);
	void Error(int line, int col, const wchar_t *s);
	void Warning(int line, int col, const wchar_t *s);
	void Warning(const wchar_t *s);
	void Exception(const wchar_t *s);

}; // Errors

namespace ModTab{class ModuleTable;}

class Parser {
public:
	ModTab::ModuleTable* modtab;
	ModTab::ModuleTable* getmodtab(){return modtab;}
//public:
	//void addParserListener(const ParserListener *const parserListener){this->parserListener=parserListener;}
private:
	enum {
		_EOF=0,
		_ident=1,
		_badString=2,
		_string=3,
		_integer=4,
		_real=5
	};
	int maxT;

	//const ParserListener *parserListener;
	Token *dummyToken;
	int errDist;
	int minErrDist;

	void SynErr(int n);
	void Get();
	void Expect(int n);
	bool StartOf(int s);
	void ExpectWeak(int n, int follow);
	bool WeakSeparator(int n, int syFol, int repFol);

public:
	Scanner *scanner;
	Errors  *errors;

	Token *t;			// last recognized token
	Token *la;			// lookahead token

CodeGenerator* getCodeGenerator(){
	  return gen;
	}

	typedef bool boolean;
	
	static const int // operators
	  illegal_operator=1, plus=2, minus=3, times=4, slash=5, equals=6, less=7, greater=8,
	  orOperation=9, notEquals=10, lessOrEqual=11, greaterOrEqual=12, in=13, is=14, divOp=15, modOp=16, ampersand=17;

	static const int // object kinds
	  var=1, proc=2;

	typedef wchar_t* characterRecord; 
	typedef wchar_t* stringRecord; 


		  
	struct ElementRangeRecord{
		ExprRecord expr1;
		boolean isrange;
		ExprRecord expr2;
	};
	
	struct SetRecord{
		boolean emptySet;
		ElementRangeRecord range;
		SetRecord* nullOrPtrToNextSet;
	};


	struct QualidentOrOptionalExprListRecord{
		boolean exprListPresent;
		ExprListRecord exprList;
		Value* calc(Parser* parser, identRecord* ident, SymbolTable &tab){
			wprintf(L"CALLING %ls.%ls(args)...", parser->modulePtr->moduleName, ident->ident_);
			return 0;
		}
	};

	struct FactorRecord_Expr: public FactorRecord{
		virtual int getFactorType(){return ft_Expr;}
		ExprRecord expr;
		virtual Value* calculate(Parser *parser, SymbolTable& tab){return expr.calculate(parser, tab);}
	};
/*
	struct DesignatorMaybeWithExprListRepeatingPartRecord{
		int clauseNumber; //[1..4]
		identRec clause1_identRec;
		ExprListRecord clause2_exprList;
		QualidentOrOptionalExprListRecord clause4_qualidentOrOptionalExprList;

		DesignatorMaybeWithExprListRepeatingPartRecord* nullOrPtrToNextDesignatorMaybeWithExprListRepeatingPartRecord;
//	("." ident 			//clauseNumber==1
//	| "[" ExprList "]" 	//clauseNumber==2
//	| "^" 				//clauseNumber==3
//	| "(" QualidentOrOptionalExprList ")" //clauseNumber==4
//	)

	};
*/
	enum ClauseEnum {cl1,cl2,cl3,cl4};

	struct DesignatorMaybeWithExprListRepeatingPartRecord{
		virtual ClauseEnum getClauseNumber()=0;
		virtual ~DesignatorMaybeWithExprListRepeatingPartRecord(){}		
		virtual Value* calc(Parser* parser, identRecord* ident, SymbolTable &tab)=0;
		DesignatorMaybeWithExprListRepeatingPartRecord* nullOrPtrToNextDesignatorMaybeWithExprListRepeatingPartRecord;

//	("." ident 			//clauseNumber==1
//	| "[" ExprList "]" 	//clauseNumber==2
//	| "^" 				//clauseNumber==3
//	| "(" QualidentOrOptionalExprList ")" //clauseNumber==4
//	)

	};

	struct DesignatorMaybeWithExprListRepeatingPartRecordCL1 : public DesignatorMaybeWithExprListRepeatingPartRecord{
		virtual ClauseEnum getClauseNumber() {return cl1;}
		virtual Value* calc(Parser* parser, identRecord* id1, SymbolTable &tab);
		identRec clause1_identRec;
		//"." ident
	};
	struct DesignatorMaybeWithExprListRepeatingPartRecordCL2 : public DesignatorMaybeWithExprListRepeatingPartRecord{
		virtual ClauseEnum getClauseNumber() {return cl2;}
		virtual ~DesignatorMaybeWithExprListRepeatingPartRecordCL2(){}
		virtual Value* calc(Parser* parser, identRecord* ident, SymbolTable &tab){return new ValueIdentAndSquareBracketedExprList(parser, tab, ident, &clause2_exprList);}
		ExprListRecord clause2_exprList;
		//"[" ExprList "]"
	};
	struct DesignatorMaybeWithExprListRepeatingPartRecordCL3 : public DesignatorMaybeWithExprListRepeatingPartRecord{
		virtual ClauseEnum getClauseNumber() {return cl3;}
		virtual Value* calc(Parser* parser, identRecord* ident, SymbolTable &tab){return new ValueIdentAndCaret(parser, tab, ident);}
	    //"^"
	};

	struct DesignatorMaybeWithExprListRepeatingPartRecordCL4 : public DesignatorMaybeWithExprListRepeatingPartRecord{
		virtual ~DesignatorMaybeWithExprListRepeatingPartRecordCL4(){}
		virtual ClauseEnum getClauseNumber() {return cl4;}
		virtual Value* calc(Parser* parser, identRecord* ident, SymbolTable &tab){
			return clause4_qualidentOrOptionalExprList.calc(parser, ident, tab);
		}
		QualidentOrOptionalExprListRecord clause4_qualidentOrOptionalExprList;
		//"(" QualidentOrOptionalExprList ")"
	};

	struct DesignatorMaybeWithExprListRecord{
		identRecord identRec;
		DesignatorMaybeWithExprListRepeatingPartRecord* nullOrPtrToNextDesignatorMaybeWithExprListRepeatingPartRecord;
		Value* calc(Parser * parser, SymbolTable &tab){
			if(nullOrPtrToNextDesignatorMaybeWithExprListRepeatingPartRecord==0){
				return new ValueOfIdent(identRec, tab.Find(identRec.ident_));
			}
			else return nullOrPtrToNextDesignatorMaybeWithExprListRepeatingPartRecord->calc(parser, &identRec, tab);
		}
	};


	struct FactorRecord_DesignatorMaybeWithExprList: public FactorRecord{
		virtual int getFactorType(){return ft_DesignatorMaybeWithExprList;}
		DesignatorMaybeWithExprListRecord r; 
		virtual Value* calculate(Parser* parser, SymbolTable& tab){return r.calc(parser, tab);}
	};
  

	struct FactorRecord_numberLiteral: public FactorRecord{
		virtual int getFactorType(){return ft_number;}
		literalNumberRecord numLiteral; 
		
		//type is not yet known, assume RandomPrecisionNumber, so we just store a string
		virtual Value* calculate(Parser* parser, SymbolTable& tab){return new ValueNumber(&numLiteral);}
	};
  
	struct FactorRecord_character: public FactorRecord{
		virtual int getFactorType(){return ft_character;}
		characterRecord ch; 
		virtual Value* calculate(Parser* parser, SymbolTable& tab){return new ValueTBD();}
	};
  
	struct FactorRecord_string: public FactorRecord{
		virtual int getFactorType(){return ft_string;}
		stringRecord s; 
		virtual Value* calculate(Parser* parser, SymbolTable& tab){return new ValueTBD();}
	};
  
	struct FactorRecord_NIL: public FactorRecord{
		virtual int getFactorType(){return ft_NIL;}
		virtual Value* calculate(Parser* parser, SymbolTable& tab){return new ValueTBD();}
	};

	struct FactorRecord_Set: public FactorRecord{
		virtual int getFactorType(){return ft_Set;}
		SetRecord set;
		virtual Value* calculate(Parser* parser, SymbolTable& tab){return new ValueTBD();}
	};
 
	struct FactorRecord_tildeFactor: public FactorRecord{
		virtual int getFactorType(){return ft_tildeFactor;}
		FactorRecord* factorPtr;
		virtual Value* calculate(Parser* parser, SymbolTable& tab){return new ValueTBD();}
	};

	static const int modifier_none=0,modifier_star=1,modifier_minus=2;
	
	struct StatementRecord{
		virtual int getStatementTypeNumber()=0;
		virtual ~StatementRecord(){}
		virtual void interpret(Parser *parser, SymbolTable &tab)=0; 
	};

	struct StatementSeqRecord{
		StatementRecord *statementPtr;
		StatementSeqRecord* nullOrPtrToNextStatementSeq;
		virtual void interpret(Parser*p, CodeGenerator &codegen, SymbolTable &tab);
		void perform(Parser *parser, SymbolTable &tab){
			StatementSeqRecord* cur = this;
			do{
				if(cur->statementPtr!=0){
					cur->statementPtr->interpret(parser, tab);
				}
				cur = cur->nullOrPtrToNextStatementSeq;
			}while(cur!=0);
		}
	};

	struct VarDeclRecord{
		IdentListRecord identList;
		TypeRecord *typePtr;
	};
	
	struct ConstDeclRecord{
		IdentDefRecord identDef;
		ConstExprRecord expr;
	};

	struct TypeDeclRecord{
		IdentDefRecord identDef;
		TypeRecord *typePtr;
	};
	
	struct ModuleImportEntryRecord{
		identRec lhs,rhs;
		boolean rhsPresent;
	};

	struct ImportListRecord{
		ModuleImportEntryRecord moduleImportEntry;
		ImportListRecord *nullOrPtrToNextModuleImportEntriesList;
	};

	struct ReceiverRecord{
		bool varSpecified;
		identRec leftIdent;
		identRec rightIdent;
	};
	
	struct OptionalReceiverRecord{
		bool receiverSpecified;
		ReceiverRecord receiver;
	};

	struct FPSectionRecord{
		bool var;
		bool const_;
		IdentList2Record identList;
		TypeRecord *typePtr;
		void addAllToScope(Parser* p){
			IdentList2Record *cur = &identList;
			do{
				p->tab->NewObj(cur->ident_, OKvar, typePtr, 0);
				cur=cur->nullOrCommaIdentList;
			}while(cur!=0);
		}
	};

	struct FPSectionsListMandatoryRecord{
		FPSectionRecord fpSection;
		FPSectionsListMandatoryRecord *next;
		void addAllToScope(Parser*p){
			FPSectionsListMandatoryRecord *cur=this;
			do{
				cur->fpSection.addAllToScope(p);
				cur=cur->next;
			}while(cur!=0);
		}
	};
	
	struct FormalParsRecord{
		//formal params
		FPSectionsListMandatoryRecord *optionalFPSectionsListPtr;

		//return type Qualident
		QualidentRecord *optionalQualidentPtr;
		void addAllToScope(Parser*p){
			if(optionalFPSectionsListPtr==0)return;
			optionalFPSectionsListPtr->addAllToScope(p);
		}
	};

	struct TypePROCEDURE: public TypeRecord{
		int getTypeNumber(){return type_number_PROCEDURE;}
		FormalParsRecord *optionalFormalParsPtr;
		virtual size_t getTypeSizeInBits(){ return sizeof(void*); }
	};


	struct OptionalFormalParsRecord{
		bool formalParsSpecified;
		FormalParsRecord formalPars;
		void addAllToScope(Parser*p){
			if(formalParsSpecified){
				formalPars.addAllToScope(p);
			}
		}
	};

	struct ForwardDeclRecord{
		OptionalReceiverRecord optionalReceiver;
		IdentDefRecord identDef;
		OptionalFormalParsRecord optionalFormalPars;
	};

	class ConstDeclRecord;
	
	struct DeclConstDO: public DataObject{
		DataObjectKind getKind(){return DeclConstDOK;}
		ConstDeclRecord *DeclConstPTR;
	};
	
	struct DeclSeqConstDeclListMandatoryRecord{
		ConstDeclRecord constDecl;
		DeclSeqConstDeclListMandatoryRecord *nullOrPtrToNextDeclSeqConstDeclListMandatory;
		void interpret(CodeGenerator &codegen, SymbolTable &tab){
			DeclSeqConstDeclListMandatoryRecord*cur = this;
			while(cur!=0){
				IdentDefRecord* curIL = &(cur->constDecl.identDef);
				IdentDefRecord* identDefPtr = curIL;
				wprintf(L"CONST %ls\n", identDefPtr->ident_);
				DeclConstDO* DO = new DeclConstDO; 
				DO->DeclConstPTR=&(cur->constDecl);
				tab.parser->tab->NewObj(identDefPtr->ident_, OKconst, 0, DO);

				cur = cur->nullOrPtrToNextDeclSeqConstDeclListMandatory;
			}
		}
	};
	struct DeclSeqTypeDeclListMandatoryRecord{
		TypeDeclRecord typeDecl;
		DeclSeqTypeDeclListMandatoryRecord *nullOrPtrToNextDeclSeqTypeDeclListMandatory;
	};
	class VarDeclRecord;
	
	struct DeclVarDO: public DataObject{
		DataObjectKind getKind(){return DeclVarDOK;}
		VarDeclRecord *DeclVarPTR;
	};
	struct DeclSeqVarDeclListMandatoryRecord{
		VarDeclRecord varDecl;
		DeclSeqVarDeclListMandatoryRecord *nullOrPtrToNextDeclSeqVarDeclListMandatory;
		virtual ~DeclSeqVarDeclListMandatoryRecord(){}
		virtual void interpret(CodeGenerator &codegen, SymbolTable &tab){
			DeclSeqVarDeclListMandatoryRecord*cur = this;
			while(cur!=0){
				IdentListRecord* curIL = &(cur->varDecl.identList);
				while(curIL!=0){
					IdentDefRecord* identDefPtr = &(curIL->identDef);
					wprintf(L"VAR %ls\n", identDefPtr->ident_);
					DeclVarDO* DO = new DeclVarDO; 
					DO->DeclVarPTR=&(cur->varDecl);
					tab.parser->tab->NewObj(identDefPtr->ident_, OKvar, cur->varDecl.typePtr, DO);
					curIL = curIL->nullOrCommaIdentList;
				}
				cur = cur->nullOrPtrToNextDeclSeqVarDeclListMandatory;
			}
		}
	};
	
	struct DeclSeqConstDeclListRecord{
		bool specified;
		DeclSeqConstDeclListMandatoryRecord constDecls; // undefined if specified==false
		void interpret(CodeGenerator &codegen, SymbolTable &tab){
			if(!specified)return;
			constDecls.interpret(codegen,tab);
		}
	};
	struct DeclSeqTypeDeclListRecord{
		bool specified;
		DeclSeqTypeDeclListMandatoryRecord typeDecls; // undefined if specified==false
	};
	struct DeclSeqVarDeclListRecord{
		bool specified;
		DeclSeqVarDeclListMandatoryRecord varDecls; // undefined if specified==false
		void interpret(CodeGenerator &codegen, SymbolTable &tab){
			if(!specified)return;
			varDecls.interpret(codegen,tab);
		}
	};

	enum DeclEnum {decl_const,decl_type,decl_var};
	
	struct DeclSeqConstTypeVarListMandatoryRecord{
		virtual DeclEnum get_decl_variant()=0;
		virtual ~DeclSeqConstTypeVarListMandatoryRecord(){}
		virtual void interpret(CodeGenerator &codegen, SymbolTable &tab){
			if(next!=0)next->interpret(codegen,tab);
		}
		DeclSeqConstTypeVarListMandatoryRecord *next;
	}; 
	
	struct DeclSeqConst : public DeclSeqConstTypeVarListMandatoryRecord{
		virtual DeclEnum get_decl_variant() {return decl_const;}
		virtual void interpret(CodeGenerator &codegen, SymbolTable &tab){
			constDeclList.interpret(codegen,tab);
		  	DeclSeqConstTypeVarListMandatoryRecord::interpret(codegen,tab);
		}
		DeclSeqConstDeclListRecord constDeclList; 
	}; 

	struct DeclSeqType : public DeclSeqConstTypeVarListMandatoryRecord{
		virtual DeclEnum get_decl_variant() {return decl_type;}
		virtual void interpret(CodeGenerator &codegen, SymbolTable &tab){
			wprintf(L"TYPE ");
		  	DeclSeqConstTypeVarListMandatoryRecord::interpret(codegen,tab);
		}
		DeclSeqTypeDeclListRecord typeDeclList;
	}; 

	struct DeclSeqVar : public DeclSeqConstTypeVarListMandatoryRecord{
		virtual DeclEnum get_decl_variant() {return decl_var;}
		virtual void interpret(CodeGenerator &codegen, SymbolTable &tab){
			varDeclList.interpret(codegen,tab);
		  	DeclSeqConstTypeVarListMandatoryRecord::interpret(codegen,tab);
		}
		DeclSeqVarDeclListRecord varDeclList;
	}; 

	//enum decl_type_enum {decl_const,decl_type,decl_var};
	
/*	struct DeclSeqConstTypeVarListMandatoryRecord{
		DeclSeqConstTypeVarListMandatoryRecord(){};
		~DeclSeqConstTypeVarListMandatoryRecord(){}
		virtual decl_type_enum decl_variant()=0;
		DeclSeqConstTypeVarListMandatoryRecord *next;
	};*/ 
	/*
	struct DeclSeqConstListMandatoryRecord{
	private: DeclSeqConstDeclListRecord* f;
	public:
		DeclSeqConstListMandatoryRecord(DeclSeqConstDeclListRecord* constDeclList):f(constDeclList){}
		~DeclSeqConstListMandatoryRecord(){}
		virtual decl_type_enum decl_variant(){return decl_const;}
		DeclSeqConstDeclListRecord* get(){return f;}
	}; 
	
	struct DeclSeqTypeListMandatoryRecord{
	private: DeclSeqTypeDeclListRecord* f;
	public:
		virtual decl_type_enum decl_variant(){return decl_type;}
		DeclSeqTypeListMandatoryRecord(DeclSeqTypeDeclListRecord* ty):f(ty){}
		~DeclSeqTypeListMandatoryRecord(){}
		DeclSeqTypeDeclListRecord* get(){return f;}
	}; 
	
	struct DeclSeqVarListMandatoryRecord{
	private: DeclSeqVarDeclListRecord* f;
	public:
		virtual decl_type_enum decl_variant(){return decl_var;}
		DeclSeqVarListMandatoryRecord(DeclSeqVarDeclListRecord* va):f(va){}
		~DeclSeqVarListMandatoryRecord(){}
		DeclSeqVarDeclListRecord* get(){return f;}	
	}; 
	*/
	struct DeclSeqConstTypeVarListRecord{
		bool specified;
		DeclSeqConstTypeVarListMandatoryRecord* constTypeVarListPtr; // undefined if specified==false
		~DeclSeqConstTypeVarListRecord(){}
		virtual void interpret(CodeGenerator &codegen, SymbolTable &tab){
			if(specified){
				constTypeVarListPtr->interpret(codegen, tab);
			} 
		}
	};

	struct DeclSeqRecord;
	
	struct ProcDeclRecord{
		OptionalReceiverRecord optionalReceiver;
		IdentDefRecord identDef;
		OptionalFormalParsRecord optionalFormalPars;
		DeclSeqRecord *declSeqPtr; 
	    bool procBodySpecifiedHere;
	    StatementSeqRecord *procBodyStmtSeq; // undefined if procBodySpecifiedHere==false 
	};

	enum declTypePF {decl_proc,decl_fwd};
	
	struct DeclSeqProcDeclFwdDeclListMandatoryRecord{
		virtual declTypePF decl_variant()=0;
		virtual ~DeclSeqProcDeclFwdDeclListMandatoryRecord(){} 
		virtual void interpret(CodeGenerator &codegen, SymbolTable &tab){
			if(next!=0)next->interpret(codegen,tab);
		}
		DeclSeqProcDeclFwdDeclListMandatoryRecord *next;
		virtual Value* callProcedure(Parser* parser)=0;
	}; 
	
	class DeclSeqProc;
	
	struct DeclSeqProcDO: public DataObject{
		DataObjectKind getKind(){return DeclSeqProcDOK;}
		DeclSeqProc *DeclSeqProcPTR;
		ForwardDeclRecord *ForwardDeclPTR;
	};
	struct DeclSeqProc : public DeclSeqProcDeclFwdDeclListMandatoryRecord{
		virtual declTypePF decl_variant(){return decl_proc;};
		virtual void interpret(CodeGenerator &codegen, SymbolTable &tab){
		  wprintf(L"PROCEDURE %ls\n", procDecl.identDef.ident_);
		  Obj* obj = tab.parser->tab->FindSilent(procDecl.identDef.ident_);
		  DeclSeqProcDO* DO = 0;
		  if (obj!=0 && obj->type!=0 && obj->kind==OKproc && obj->type->getTypeNumber()==type_number_PROCEDURE){
			  DO = (DeclSeqProcDO*) obj->data;
			  DO->DeclSeqProcPTR=this;
		  }else{
			  if(obj==0){
				  DO = new DeclSeqProcDO; abortIfNull(DO);
				  DO->DeclSeqProcPTR=this;
				  DO->ForwardDeclPTR=0;
	    		  tab.parser->tab->NewObj(procDecl.identDef.ident_, OKproc, new TypePROCEDURE, DO);
			  }else{
			    const int sz=512;
				wchar_t str[sz];
				coco_swprintf(str, sz, L"Cannot define procedure %ls: some object with a name %ls already declared", procDecl.identDef.ident_, procDecl.identDef.ident_);
				tab.parser->tab->Err(str);
			  }
		  }
		  /*
		  Obj* scope = tab.OpenScope();
		  if(DO!=0){
			  DO->scope = scope;
		  }
		  tab.CloseScope();
		  */
		  DeclSeqProcDeclFwdDeclListMandatoryRecord::interpret(codegen,tab);
		}
		ProcDeclRecord procDecl;
		virtual Value* callProcedure(Parser* parser){
			wprintf(L"CALLING %ls.%ls... ", parser->modulePtr->moduleName, procDecl.identDef.ident_);
  		    Obj* scope = parser->tab->OpenScope();
  		    //OptionalFormalParsRecord optionalFormalPars
  		    procDecl.optionalFormalPars.addAllToScope(parser);
  		    procDecl.declSeqPtr->interpret(parser, *parser->getCodeGenerator(),*(parser->tab));
			if(procDecl.procBodySpecifiedHere){
				if(procDecl.procBodyStmtSeq!=0){
					procDecl.procBodyStmtSeq->perform(parser, *(parser->tab));
				}
			}
			Value* v = new ValueTBD();
			parser->tab->CloseScope();
		    return v;
		}
	};
	
	struct DeclSeqFwd : public DeclSeqProcDeclFwdDeclListMandatoryRecord{
		virtual declTypePF decl_variant(){return decl_fwd;};
		virtual void interpret(CodeGenerator &codegen, SymbolTable &tab){
		  wprintf(L"FORWARD ^%ls\n",fwdDecl.identDef.ident_);
		  DeclSeqProcDO* DO = new DeclSeqProcDO;
		  DO->DeclSeqProcPTR=0;
		  DO->ForwardDeclPTR=&fwdDecl;
		  tab.parser->tab->NewObj(fwdDecl.identDef.ident_, OKproc, new TypePROCEDURE, DO);
		  DeclSeqProcDeclFwdDeclListMandatoryRecord::interpret(codegen,tab);
		}
		ForwardDeclRecord fwdDecl;  
		virtual Value* callProcedure(Parser* parser){
		  	return new ValueTBD();
		}
	};
	
	struct DeclSeqProcDeclFwdDeclListRecord{
		bool specified;
		DeclSeqProcDeclFwdDeclListMandatoryRecord* procDeclFwdDeclListPtr; // undefined if specified==false
		virtual ~DeclSeqProcDeclFwdDeclListRecord(){}
		virtual void interpret(CodeGenerator& cg, SymbolTable& tab){
		  	if(specified){
		  		procDeclFwdDeclListPtr->interpret(cg,tab);
		  	}
		}
	};
	
	struct DeclSeqRecord{
		DeclSeqConstTypeVarListRecord ctvList;
		DeclSeqProcDeclFwdDeclListRecord pfList;
		StatementSeqRecord * moduleLoad;
		StatementSeqRecord * moduleUnload;
		virtual ~DeclSeqRecord(){}
		virtual void interpret(Parser*p, CodeGenerator &codegen, SymbolTable &tab){
			if(moduleLoad!=0)moduleLoad->interpret(p,codegen, tab);
			ctvList.interpret(codegen, tab);
			pfList.interpret(codegen, tab);
		}
	};

	static const int
		 stmtTypeNumber_EmptyStmt=0
		,stmtTypeNumber_EXPR_OR_ASSIGN=1
		,stmtTypeNumber_IF=2
		,stmtTypeNumber_CASE=3
		,stmtTypeNumber_WHILE=4
		,stmtTypeNumber_REPEAT=5
		,stmtTypeNumber_FOR=6
		,stmtTypeNumber_LOOP=7
		,stmtTypeNumber_WITH=8
		,stmtTypeNumber_EXIT=9
		,stmtTypeNumber_RETURN=10
		;
	struct Stmt_EmptyStmt:public StatementRecord{
	  virtual int getStatementTypeNumber(){return stmtTypeNumber_EmptyStmt;} 
  	virtual void interpret(Parser*parser, SymbolTable &tab){}; 
	};
	struct Stmt_EXPR_OR_ASSIGN:public StatementRecord{
	  	virtual int getStatementTypeNumber(){return stmtTypeNumber_EXPR_OR_ASSIGN;}
	  	/*
			Expr<(*psea).lhsExpr> 
			( 
													(. psea->assignment=false; .) 
			| 										(. psea->assignment=true; .) 
				":=" Expr<(*psea).rhsExpr> 
			) 
	  	 */
	  	ExprRecord lhsExpr;
	  	bool assignment;
	  	ExprRecord rhsExpr; //undefined iff !assignment 
	  	virtual void interpret(Parser *parser, SymbolTable &tab){
	  		if(assignment){
		  		wprintf(L"\nASSIGNMENT TO ");
		  		Value * lvalue = lhsExpr.calculate(parser, tab);
		  		PRINT_VALUE(lvalue);
		  		Value * rvalue = rhsExpr.calculate(parser, tab);
		  		wprintf(L" OF ");
		  		PRINT_VALUE(rvalue);
		  		wprintf(L"\n");
	  		}else{
		  		lhsExpr.calculate(parser, tab);
	  		}
	  	}; 
	};
	struct MandatoryELSIFsListRecord{
		ExprRecord expr;
		StatementSeqRecord *thenStmtSeq; 
		MandatoryELSIFsListRecord *optionalElsifsListPtr;
	};
	struct Stmt_IF:public StatementRecord{
	 	virtual int getStatementTypeNumber(){return stmtTypeNumber_IF;} 
	  	virtual void interpret(Parser *parser, SymbolTable &tab){wprintf(L"IF ... END ");}; 
	    ExprRecord expr;
		StatementSeqRecord *thenStmtSeq;
		MandatoryELSIFsListRecord *optionalElsifsListPtr;
	    StatementSeqRecord *optionalElsePtr;
	};
	struct CaseLabelsRecord{
		ConstExprRecord constExpr1; 
		bool secondConstExprPresent;
		ConstExprRecord constExpr2;
	};
	struct CaseLabelsListsRecord{
		CaseLabelsRecord caseLabelsNth;
		CaseLabelsListsRecord *optionalFurtherCaseLabelsListsPtr;
	};
	struct CaseRecord{
		bool emptyCase;
		CaseLabelsListsRecord caseLabelsLists;
		StatementSeqRecord *stmtSeq;
	};
	struct CasesRecord{
		CaseRecord caseNth;
		CasesRecord *optionalOtherCasesPtr;
	};
	struct Stmt_CASE:public StatementRecord{
	  	virtual int getStatementTypeNumber(){return stmtTypeNumber_CASE;} 
	  	virtual void interpret(Parser *parser, SymbolTable &tab){wprintf(L"CASE... ");}; 
		ExprRecord expr;
		CaseRecord caseFirst;
		CasesRecord *optionalOtherCasesPtr;
		StatementSeqRecord *optionalElsePtr;
	};
	struct Stmt_WHILE:public StatementRecord{
		virtual int getStatementTypeNumber(){return stmtTypeNumber_WHILE;} 
	  	virtual void interpret(Parser *parser, SymbolTable &tab){wprintf(L"WHILE... ");}; 
	    ExprRecord expr;
	    StatementSeqRecord *whileBodyStatementSeq;
	};
	struct Stmt_REPEAT:public StatementRecord{
		virtual int getStatementTypeNumber(){return stmtTypeNumber_REPEAT;} 
	  	virtual void interpret(Parser *parser, SymbolTable &tab){wprintf(L"REPEAT UNTIL... ");}; 
	    StatementSeqRecord *repeatBodyStatementSeq;
	    ExprRecord expr;
	};
	struct Stmt_FOR:public StatementRecord{
	  virtual int getStatementTypeNumber(){return stmtTypeNumber_FOR;} 
	  	virtual void interpret(Parser *parser, SymbolTable &tab){wprintf(L"FOR... ");}; 
	  identRec forCounterVariableName;
	  ExprRecord forCounterVariableInitialValueExpr;
	  ExprRecord forCounterVariableToValueExpr;
	  bool bySpecified;
	  ConstExprRecord byValueConstExpr; 
	  StatementSeqRecord *forStatementSeq; 
	};
	struct Stmt_LOOP:public StatementRecord{
		virtual int getStatementTypeNumber(){return stmtTypeNumber_LOOP;} 
	  	virtual void interpret(Parser *parser, SymbolTable &tab){wprintf(L"LOOP... ");}; 
	    StatementSeqRecord *loopStatementSeq;
	};
	struct GuardRecord{
		QualidentRecord qualident1, qualident2;
	};
	struct FurtherWithClausesRecord{
		GuardRecord guard; 
		StatementSeqRecord *statementSeq;
		FurtherWithClausesRecord *next;
	};
	struct Stmt_WITH:public StatementRecord{
		virtual int getStatementTypeNumber(){return stmtTypeNumber_WITH;} 
	  	virtual void interpret(Parser *parser, SymbolTable &tab){wprintf(L"WITH... ");}; 
	    GuardRecord firstGuard;
	    StatementSeqRecord *firstStatementSeq;
		FurtherWithClausesRecord *optionalFurtherWithClausesPtr;
		StatementSeqRecord *optionalElsePtr;
	};
	struct Stmt_EXIT:public StatementRecord{
		virtual int getStatementTypeNumber(){return stmtTypeNumber_EXIT;} 
	  	virtual void interpret(Parser *parser, SymbolTable &tab){wprintf(L"EXIT ");}; 
	};
	struct Stmt_RETURN:public StatementRecord{
		virtual int getStatementTypeNumber(){return stmtTypeNumber_RETURN;} 
	  	virtual void interpret(Parser *parser, SymbolTable &tab){wprintf(L"RETURN... ");}; 
		bool exprPresent;
		ExprRecord expr;
	};

	struct ModuleRecord{
		ModuleRecord(){
			initialized=false;
		}
		wchar_t* moduleName;
		ImportListRecord *importListPtr;
		DeclSeqRecord declSeq;
		StatementSeqRecord *stmtSeq; //may be null if there's no MODULE Init section
		bool initialized;
	};

	ModuleRecord *modulePtr;
public:
	SymbolTable   *tab;
	CodeGenerator *gen;

	void Err(const wchar_t* msg) {
		errors->Error(la->line, la->col, msg);
	}

	void InitDeclarations() { // it must exist
	}


  

  
/*--------------------------------------------------------------------------*/


	Parser(Scanner *scanner, Errors* errors_);
	~Parser();
	void SemErr(const wchar_t* msg);

	void character();
	void number(literalNumberRecord &r);
	void IntegerRec(wchar_t* &tok);
	void RealRec(wchar_t* &tok);
	void Relation(int &op);
	void AddOp(int &op);
	void MulOp(int &op);
	void Expr(ExprRecord &expr);
	void SimpleExpr(SimpleExprRecord &e);
	void ConstExpr(ConstExprRecord &cexpr);
	void ModuleImportEntry(ModuleImportEntryRecord &r);
	void Ident(wchar_t* &tok);
	void ModuleImportEntryList(ImportListRecord &r);
	void ImportList(ImportListRecord &r);
	void DeclSeqConstDeclListMandatory(DeclSeqConstDeclListMandatoryRecord &r);
	void ConstDecl(ConstDeclRecord &r);
	void DeclSeqConstDeclList(DeclSeqConstDeclListRecord &r);
	void DeclSeqTypeDeclListMandatory(DeclSeqTypeDeclListMandatoryRecord &r);
	void TypeDecl(TypeDeclRecord &r);
	void DeclSeqTypeDeclList(DeclSeqTypeDeclListRecord &r);
	void DeclSeqVarDeclListMandatory(DeclSeqVarDeclListMandatoryRecord &r);
	void VarDecl(VarDeclRecord &r);
	void DeclSeqVarDeclList(DeclSeqVarDeclListRecord &r);
	void DeclSeqConstTypeVarListMandatory(DeclSeqConstTypeVarListMandatoryRecord *&r);
	void DeclSeqConstTypeVarList(DeclSeqConstTypeVarListRecord &r);
	void DeclSeqProcDeclFwdDeclListMandatory(DeclSeqProcDeclFwdDeclListMandatoryRecord *&r);
	void ProcDecl(ProcDeclRecord &r);
	void ForwardDecl(ForwardDeclRecord &r);
	void DeclSeqProcDeclFwdDeclList(DeclSeqProcDeclFwdDeclListRecord &r);
	void DeclSeq(DeclSeqRecord &r);
	void StatementSeq(StatementSeqRecord *&r);
	void IdentDef(IdentDefRecord &r);
	void Type(TypeRecord *&ptrToTypeRecord);
	void IdentList(IdentListRecord &r);
	void OptionalReceiver(OptionalReceiverRecord &r);
	void Receiver(ReceiverRecord &r);
	void OptionalFormalPars(OptionalFormalParsRecord &r);
	void FormalPars(FormalParsRecord &r);
	void FPSectionsListMandatory(FPSectionsListMandatoryRecord &r);
	void FPSection(FPSectionRecord &r);
	void Qualident(QualidentRecord &r);
	void IdentList2(IdentList2Record &r);
	void TypeArrayConstExprListMandatory(TypeArrayConstExprListMandatoryRecord &r);
	void TypeArray(TypeARRAY &r);
	void MandatoryFieldsList(MandatoryFieldsListRecord &r);
	void FieldList(FieldListRecord &r);
	void TypeRecord_(TypeRECORD &r);
	void TypeProcedure(TypePROCEDURE &r);
	void Statement(StatementRecord*&ptrToStmtRecord);
	void CaseLabelsLists(CaseLabelsListsRecord &r);
	void CaseLabels(CaseLabelsRecord &r);
	void Case(CaseRecord &r);
	void Guard(GuardRecord &r);
	void SimpleExprAddOpClause(SimpleExprAddOpRecord &r);
	void Term(TermRecord &t);
	void TermMulOpClause(TermMulOpRecord &r);
	void Factor(FactorRecord *&factorPtr);
	void DesignatorMaybeWithExprList(DesignatorMaybeWithExprListRecord &r);
	void Character(wchar_t* &tok);
	void String(wchar_t* &tok);
	void Set(SetRecord &r);
	void DesignatorMaybeWithExprListRepeatingPartClause(DesignatorMaybeWithExprListRepeatingPartRecord *&r);
	void IdentRec(wchar_t* &tok);
	void ExprList(ExprListRecord &r);
	void QualidentOrOptionalExprList(QualidentOrOptionalExprListRecord &r);
	void ElementRangeList(SetRecord &r);
	void Element(ElementRangeRecord &r);
	void MandatoryELSIFsList(MandatoryELSIFsListRecord &r);
	void Cases(CasesRecord &r);
	void FurtherWithClauses(FurtherWithClausesRecord &r);
	void Module(ModuleRecord &r);
	void Integer(wchar_t* &tok);
	void Real(wchar_t* &tok);
	void Oberon();

	void Parse();

}; // end Parser

/* } // namespace
 */

#endif

