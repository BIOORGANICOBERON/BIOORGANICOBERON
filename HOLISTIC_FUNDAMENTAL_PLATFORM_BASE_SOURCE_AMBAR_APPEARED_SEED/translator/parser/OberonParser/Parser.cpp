/*----------------------------------------------------------------------
Oberon suite
Copyright (c) 2012, 2014 Evgeniy Grigorievitch Philippov. All Rights Reserved.
-----------------------------------------------------------------------*/


#include "Parser.h"
#include "Scanner.h"
#include "ModuleTable.h"
#include "SymbolTable.h"
#include "../generator/CodeGenerator.h"


/* namespace Oberon {
 */

void Parser::SynErr(int n) {
	//if (errDist >= minErrDist)
		errors->SynErr(la->line, la->col, n);
	errDist = 0;
}

void Parser::SemErr(const wchar_t* msg) {
	//if (errDist >= minErrDist)
		errors->Error(t->line, t->col, msg);
	errDist = 0;
}

void Parser::Get() {
	for (;;) {
		t = la;
		la = scanner->Scan();
		if (la->kind <= maxT) { ++errDist; break; }

		if (dummyToken != t) {
			dummyToken->kind = t->kind;
			dummyToken->pos = t->pos;
			dummyToken->col = t->col;
			dummyToken->line = t->line;
			dummyToken->next = NULL;
			coco_string_delete(dummyToken->val);
			dummyToken->val = coco_string_create(t->val);
			t = dummyToken;
		}
		la = t;
	}
}

void Parser::Expect(int n) {
	if (la->kind==n) Get(); else { SynErr(n); }
}

void Parser::ExpectWeak(int n, int follow) {
	if (la->kind == n) Get();
	else {
		SynErr(n);
		while (!StartOf(follow)) Get();
	}
}

bool Parser::WeakSeparator(int n, int syFol, int repFol) {
	if (la->kind == n) {Get(); return true;}
	else if (StartOf(repFol)) {return false;}
	else {
		SynErr(n);
		while (!(StartOf(syFol) || StartOf(repFol) || StartOf(0))) {
			Get();
		}
		return StartOf(syFol);
	}
}

void Parser::character() {
		Expect(_string);
}

void Parser::number(literalNumberRecord &r) {
		if (la->kind == _integer) {
			IntegerRec(r.tokenString);
			r.literal_type=literal_int; 
		} else if (la->kind == _real) {
			RealRec(r.tokenString);
			r.literal_type=literal_real; 
		} else SynErr(70);
}

void Parser::IntegerRec(wchar_t* &tok) {
		Integer(tok);
}

void Parser::RealRec(wchar_t* &tok) {
		Real(tok);
}

void Parser::Relation(int &op) {
		op = illegal_operator; 
		switch (la->kind) {
		case 6 /* "=" */: {
			Get();
			op = equals; 
			break;
		}
		case 7 /* "#" */: {
			Get();
			op = notEquals; 
			break;
		}
		case 8 /* "<" */: {
			Get();
			op = less; 
			break;
		}
		case 9 /* "<=" */: {
			Get();
			op = lessOrEqual; 
			break;
		}
		case 10 /* ">" */: {
			Get();
			op = greater; 
			break;
		}
		case 11 /* ">=" */: {
			Get();
			op = greaterOrEqual; 
			break;
		}
		case 12 /* "IN" */: {
			Get();
			op = in; 
			break;
		}
		case 13 /* "IS" */: {
			Get();
			op = is; 
			break;
		}
		default: SynErr(71); break;
		}
}

void Parser::AddOp(int &op) {
		op = illegal_operator; 
		if (la->kind == 14 /* "+" */) {
			Get();
			op = plus; 
		} else if (la->kind == 15 /* "-" */) {
			Get();
			op = minus; 
		} else if (la->kind == 16 /* "OR" */) {
			Get();
			op = orOperation; 
		} else SynErr(72);
}

void Parser::MulOp(int &op) {
		op = illegal_operator; 
		if (la->kind == 17 /* "*" */) {
			Get();
			op = times; 
		} else if (la->kind == 18 /* "/" */) {
			Get();
			op = slash; 
		} else if (la->kind == 19 /* "DIV" */) {
			Get();
			op = divOp; 
		} else if (la->kind == 20 /* "MOD" */) {
			Get();
			op = modOp; 
		} else if (la->kind == 21 /* "&" */) {
			Get();
			op = ampersand; 
		} else SynErr(73);
}

void Parser::Expr(ExprRecord &expr) {
		SimpleExpr(expr.lhs);
		if (StartOf(1)) {
			expr.opAndRhsPresent=true; 
			Relation(expr.op);
			SimpleExpr(expr.rhs);
		} else if (StartOf(2)) {
			expr.opAndRhsPresent=false; 
		} else SynErr(74);
}

void Parser::SimpleExpr(SimpleExprRecord &e) {
		if (StartOf(3)) {
			if (StartOf(4)) {
				e.signum=false; 
			} else {
				Get();
				e.minus=false; e.signum=true; 
			}
		} else if (la->kind == 15 /* "-" */) {
			Get();
			e.minus=true; e.signum=true; 
		} else SynErr(75);
		Term(e.term);
		if (StartOf(5)) {
			e.nullOrNextSimpleExprAddOpRecord=0; 
		} else if (la->kind == 14 /* "+" */ || la->kind == 15 /* "-" */ || la->kind == 16 /* "OR" */) {
			e.nullOrNextSimpleExprAddOpRecord=new SimpleExprAddOpRecord(); abortIfNull(e.nullOrNextSimpleExprAddOpRecord); 
			SimpleExprAddOpClause(*e.nullOrNextSimpleExprAddOpRecord);
		} else SynErr(76);
}

void Parser::ConstExpr(ConstExprRecord &cexpr) {
		cexpr.valueHasBeenCalculated=false; 
		Expr(cexpr.expr);
}

void Parser::ModuleImportEntry(ModuleImportEntryRecord &r) {
		Ident(r.lhs);
		if (la->kind == 23 /* "," */ || la->kind == 25 /* ";" */) {
			r.rhsPresent=false; 
		} else if (la->kind == 22 /* ":=" */) {
			r.rhsPresent=true; 
			Get();
			Ident(r.rhs);
		} else SynErr(77);
}

void Parser::Ident(wchar_t* &tok) {
		Expect(_ident);
		tok = coco_string_create(t->val); 
}

void Parser::ModuleImportEntryList(ImportListRecord &r) {
		ModuleImportEntry(r.moduleImportEntry);
		if (la->kind == 25 /* ";" */) {
			r.nullOrPtrToNextModuleImportEntriesList=0; 
		} else if (la->kind == 23 /* "," */) {
			r.nullOrPtrToNextModuleImportEntriesList=new ImportListRecord(); abortIfNull(r.nullOrPtrToNextModuleImportEntriesList); 
			Get();
			ModuleImportEntryList(*(r.nullOrPtrToNextModuleImportEntriesList));
		} else SynErr(78);
}

void Parser::ImportList(ImportListRecord &r) {
		Expect(24 /* "IMPORT" */);
		ModuleImportEntryList(r);
		Expect(25 /* ";" */);
}

void Parser::DeclSeqConstDeclListMandatory(DeclSeqConstDeclListMandatoryRecord &r) {
		ConstDecl(r.constDecl);
		Expect(25 /* ";" */);
		if (StartOf(6)) {
			r.nullOrPtrToNextDeclSeqConstDeclListMandatory=0; 
		} else if (la->kind == _ident) {
			r.nullOrPtrToNextDeclSeqConstDeclListMandatory=new DeclSeqConstDeclListMandatoryRecord(); 
			abortIfNull(r.nullOrPtrToNextDeclSeqConstDeclListMandatory);
			
			DeclSeqConstDeclListMandatory(*(r.nullOrPtrToNextDeclSeqConstDeclListMandatory));
		} else SynErr(79);
}

void Parser::ConstDecl(ConstDeclRecord &r) {
		IdentDef(r.identDef);
		Expect(6 /* "=" */);
		ConstExpr(r.expr);
}

void Parser::DeclSeqConstDeclList(DeclSeqConstDeclListRecord &r) {
		if (StartOf(6)) {
			r.specified=false; 
		} else if (la->kind == _ident) {
			r.specified=true; 
			DeclSeqConstDeclListMandatory(r.constDecls);
		} else SynErr(80);
}

void Parser::DeclSeqTypeDeclListMandatory(DeclSeqTypeDeclListMandatoryRecord &r) {
		TypeDecl(r.typeDecl);
		Expect(25 /* ";" */);
		if (StartOf(6)) {
			r.nullOrPtrToNextDeclSeqTypeDeclListMandatory=0; 
		} else if (la->kind == _ident) {
			r.nullOrPtrToNextDeclSeqTypeDeclListMandatory=new DeclSeqTypeDeclListMandatoryRecord(); 
			abortIfNull(r.nullOrPtrToNextDeclSeqTypeDeclListMandatory);
			
			DeclSeqTypeDeclListMandatory(*(r.nullOrPtrToNextDeclSeqTypeDeclListMandatory));
		} else SynErr(81);
}

void Parser::TypeDecl(TypeDeclRecord &r) {
		IdentDef(r.identDef);
		Expect(6 /* "=" */);
		Type(r.typePtr);
}

void Parser::DeclSeqTypeDeclList(DeclSeqTypeDeclListRecord &r) {
		if (StartOf(6)) {
			r.specified=false; 
		} else if (la->kind == _ident) {
			r.specified=true; 
			DeclSeqTypeDeclListMandatory(r.typeDecls);
		} else SynErr(82);
}

void Parser::DeclSeqVarDeclListMandatory(DeclSeqVarDeclListMandatoryRecord &r) {
		VarDecl(r.varDecl);
		Expect(25 /* ";" */);
		if (StartOf(6)) {
			r.nullOrPtrToNextDeclSeqVarDeclListMandatory=0; 
		} else if (la->kind == _ident) {
			r.nullOrPtrToNextDeclSeqVarDeclListMandatory=new DeclSeqVarDeclListMandatoryRecord(); 
			abortIfNull(r.nullOrPtrToNextDeclSeqVarDeclListMandatory);
			
			DeclSeqVarDeclListMandatory(*(r.nullOrPtrToNextDeclSeqVarDeclListMandatory));
		} else SynErr(83);
}

void Parser::VarDecl(VarDeclRecord &r) {
		IdentList(r.identList);
		Expect(36 /* ":" */);
		Type(r.typePtr);
}

void Parser::DeclSeqVarDeclList(DeclSeqVarDeclListRecord &r) {
		if (StartOf(6)) {
			r.specified=false; 
		} else if (la->kind == _ident) {
			r.specified=true; 
			DeclSeqVarDeclListMandatory(r.varDecls);
		} else SynErr(84);
}

void Parser::DeclSeqConstTypeVarListMandatory(DeclSeqConstTypeVarListMandatoryRecord *&r) {
		r=0; 
		if (la->kind == 26 /* "CONST" */) {
			DeclSeqConst* rr = new DeclSeqConst(); abortIfNull(rr); r=rr; 
			Get();
			DeclSeqConstDeclList((*rr).constDeclList);
		} else if (la->kind == 27 /* "TYPE" */) {
			DeclSeqType* rr = new DeclSeqType(); abortIfNull(rr);  r=rr; 
			Get();
			DeclSeqTypeDeclList((*rr).typeDeclList);
		} else if (la->kind == 28 /* "VAR" */) {
			DeclSeqVar* rr = new DeclSeqVar(); abortIfNull(rr);  r=rr; 
			Get();
			DeclSeqVarDeclList((*rr).varDeclList);
		} else SynErr(85);
		if(r!=0){ 
		if (StartOf(7)) {
			r->next=0; 
		} else if (la->kind == 26 /* "CONST" */ || la->kind == 27 /* "TYPE" */ || la->kind == 28 /* "VAR" */) {
			DeclSeqConstTypeVarListMandatory((*r).next);
		} else SynErr(86);
		} 
}

void Parser::DeclSeqConstTypeVarList(DeclSeqConstTypeVarListRecord &r) {
		if (StartOf(7)) {
			r.specified=false; 
		} else if (la->kind == 26 /* "CONST" */ || la->kind == 27 /* "TYPE" */ || la->kind == 28 /* "VAR" */) {
			r.specified=true; 
			DeclSeqConstTypeVarListMandatory(r.constTypeVarListPtr);
		} else SynErr(87);
}

void Parser::DeclSeqProcDeclFwdDeclListMandatory(DeclSeqProcDeclFwdDeclListMandatoryRecord *&r) {
		Expect(29 /* "PROCEDURE" */);
		r=0; 
		if (la->kind == _ident || la->kind == 38 /* "(" */) {
			DeclSeqProc*rr=new DeclSeqProc(); abortIfNull(rr); r=rr; 
			ProcDecl((*rr).procDecl);
		} else if (la->kind == 37 /* "^" */) {
			DeclSeqFwd*rr=new DeclSeqFwd(); abortIfNull(rr); r=rr; 
			ForwardDecl((*rr).fwdDecl);
		} else SynErr(88);
		Expect(25 /* ";" */);
		if(r!=0){ 
		if (la->kind == 30 /* "ON" */ || la->kind == 33 /* "BEGIN" */ || la->kind == 34 /* "END" */) {
			r->next=0; 
		} else if (la->kind == 29 /* "PROCEDURE" */) {
			DeclSeqProcDeclFwdDeclListMandatory((*r).next);
		} else SynErr(89);
		} 
}

void Parser::ProcDecl(ProcDeclRecord &r) {
		identRec endName; 
		OptionalReceiver(r.optionalReceiver);
		IdentDef(r.identDef);
		OptionalFormalPars(r.optionalFormalPars);
		Expect(25 /* ";" */);
		r.declSeqPtr=new DeclSeqRecord(); abortIfNull(r.declSeqPtr); 
		DeclSeq(*(r.declSeqPtr));
		if (la->kind == 34 /* "END" */) {
			r.procBodySpecifiedHere=false; 
		} else if (la->kind == 33 /* "BEGIN" */) {
			r.procBodySpecifiedHere=true; 
			Get();
			StatementSeq(r.procBodyStmtSeq);
		} else SynErr(90);
		Expect(34 /* "END" */);
		Ident(endName);
		if(!coco_string_equal(endName, r.identDef.ident_)){
		Err(L"PROCEDURE identDef ... END ident: idents do not match."); 
		} 
		
}

void Parser::ForwardDecl(ForwardDeclRecord &r) {
		Expect(37 /* "^" */);
		OptionalReceiver(r.optionalReceiver);
		IdentDef(r.identDef);
		OptionalFormalPars(r.optionalFormalPars);
}

void Parser::DeclSeqProcDeclFwdDeclList(DeclSeqProcDeclFwdDeclListRecord &r) {
		if (la->kind == 30 /* "ON" */ || la->kind == 33 /* "BEGIN" */ || la->kind == 34 /* "END" */) {
			r.specified=false; 
		} else if (la->kind == 29 /* "PROCEDURE" */) {
			r.specified=true; 
			DeclSeqProcDeclFwdDeclListMandatory(r.procDeclFwdDeclListPtr);
		} else SynErr(91);
}

void Parser::DeclSeq(DeclSeqRecord &r) {
		DeclSeqConstTypeVarList(r.ctvList);
		DeclSeqProcDeclFwdDeclList(r.pfList);
		r.moduleLoad=r.moduleUnload=0; 
		if (la->kind == 30 /* "ON" */) {
			Get();
			Expect(31 /* "MODULE" */);
			Expect(32 /* "LOAD" */);
			Expect(33 /* "BEGIN" */);
			StatementSeq(r.moduleLoad);
			Expect(34 /* "END" */);
			Expect(31 /* "MODULE" */);
			Expect(32 /* "LOAD" */);
			if (la->kind == 25 /* ";" */) {
				Get();
			}
		}
		if (la->kind == 30 /* "ON" */) {
			Get();
			Expect(31 /* "MODULE" */);
			Expect(35 /* "UNLOAD" */);
			Expect(33 /* "BEGIN" */);
			StatementSeq(r.moduleUnload);
			Expect(34 /* "END" */);
			Expect(31 /* "MODULE" */);
			Expect(35 /* "UNLOAD" */);
			if (la->kind == 25 /* ";" */) {
				Get();
			}
		}
}

void Parser::StatementSeq(StatementSeqRecord *&r) {
		r = new StatementSeqRecord(); abortIfNull(r); 
		Statement((*r).statementPtr);
		if (StartOf(8)) {
			(*r).nullOrPtrToNextStatementSeq=0; 
		} else if (la->kind == 25 /* ";" */) {
			(*r).nullOrPtrToNextStatementSeq=new StatementSeqRecord(); abortIfNull((*r).nullOrPtrToNextStatementSeq); 
			Get();
			StatementSeq((*r).nullOrPtrToNextStatementSeq);
		} else SynErr(92);
}

void Parser::IdentDef(IdentDefRecord &r) {
		IdentRec(r.ident_);
		if (StartOf(9)) {
			r.modifier=modifier_none; 
		} else if (la->kind == 17 /* "*" */) {
			Get();
			r.modifier=modifier_star; 
		} else if (la->kind == 15 /* "-" */) {
			Get();
			r.modifier=modifier_minus; 
		} else SynErr(93);
}

void Parser::Type(TypeRecord *&ptrToTypeRecord) {
		TypeQualident* ptq;  TypeARRAY* pta; TypeRECORD* ptrec; TypePOINTER* ptp; TypePROCEDURE *ptproc; 
		if (la->kind == _ident) {
			QualidentRecord qr;  
			Qualident(qr);
			ptrToTypeRecord=ptq=new TypeQualident(qr); abortIfNull(ptrToTypeRecord);
		} else if (la->kind == 41 /* "ARRAY" */) {
			ptrToTypeRecord=pta=new TypeARRAY(); abortIfNull(ptrToTypeRecord); 
			Get();
			TypeArray(*pta);
		} else if (la->kind == 42 /* "RECORD" */) {
			ptrToTypeRecord=ptrec=new TypeRECORD(); abortIfNull(ptrToTypeRecord); 
			Get();
			TypeRecord_(*ptrec);
			Expect(34 /* "END" */);
		} else if (la->kind == 43 /* "POINTER" */) {
			ptrToTypeRecord=ptp=new TypePOINTER(); abortIfNull(ptrToTypeRecord); 
			Get();
			Expect(44 /* "TO" */);
			Type((*ptp).pointedTypePtr);
		} else if (la->kind == 29 /* "PROCEDURE" */) {
			ptrToTypeRecord=ptproc=new TypePROCEDURE(); abortIfNull(ptrToTypeRecord); 
			Get();
			TypeProcedure(*ptproc);
		} else SynErr(94);
}

void Parser::IdentList(IdentListRecord &r) {
		IdentDef(r.identDef);
		if (la->kind == 36 /* ":" */) {
			r.nullOrCommaIdentList=0; 
		} else if (la->kind == 23 /* "," */) {
			r.nullOrCommaIdentList=new IdentListRecord(); abortIfNull(r.nullOrCommaIdentList); 
			Get();
			IdentList(*(r.nullOrCommaIdentList));
		} else SynErr(95);
}

void Parser::OptionalReceiver(OptionalReceiverRecord &r) {
		if (la->kind == _ident) {
			r.receiverSpecified=false; 
		} else if (la->kind == 38 /* "(" */) {
			r.receiverSpecified=true; 
			Receiver(r.receiver);
		} else SynErr(96);
}

void Parser::Receiver(ReceiverRecord &r) {
		Expect(38 /* "(" */);
		if (la->kind == _ident) {
			r.varSpecified=false; 
		} else if (la->kind == 28 /* "VAR" */) {
			r.varSpecified=true; 
			Get();
		} else SynErr(97);
		Ident(r.leftIdent);
		Expect(36 /* ":" */);
		Ident(r.rightIdent);
		Expect(39 /* ")" */);
}

void Parser::OptionalFormalPars(OptionalFormalParsRecord &r) {
		if (la->kind == 25 /* ";" */) {
			r.formalParsSpecified=false; 
		} else if (la->kind == 38 /* "(" */) {
			r.formalParsSpecified=true; 
			FormalPars(r.formalPars);
		} else SynErr(98);
}

void Parser::FormalPars(FormalParsRecord &r) {
		Expect(38 /* "(" */);
		if (la->kind == 39 /* ")" */) {
			r.optionalFPSectionsListPtr=0; 
		} else if (la->kind == _ident || la->kind == 26 /* "CONST" */ || la->kind == 28 /* "VAR" */) {
			r.optionalFPSectionsListPtr=new FPSectionsListMandatoryRecord(); abortIfNull(r.optionalFPSectionsListPtr); 
			FPSectionsListMandatory(*(r.optionalFPSectionsListPtr));
		} else SynErr(99);
		Expect(39 /* ")" */);
		if (la->kind == 25 /* ";" */ || la->kind == 34 /* "END" */ || la->kind == 39 /* ")" */) {
			r.optionalQualidentPtr=0; 
		} else if (la->kind == 36 /* ":" */) {
			r.optionalQualidentPtr=new QualidentRecord(); abortIfNull(r.optionalQualidentPtr); 
			Get();
			Qualident(*(r.optionalQualidentPtr));
		} else SynErr(100);
}

void Parser::FPSectionsListMandatory(FPSectionsListMandatoryRecord &r) {
		FPSection(r.fpSection);
		if (la->kind == 39 /* ")" */) {
			r.next=0; 
		} else if (la->kind == 25 /* ";" */) {
			r.next=new FPSectionsListMandatoryRecord(); abortIfNull(r.next); 
			Get();
			FPSectionsListMandatory(*(r.next));
		} else SynErr(101);
}

void Parser::FPSection(FPSectionRecord &r) {
		r.var=r.const_=false; 
		if (la->kind == _ident) {
		} else if (la->kind == 28 /* "VAR" */) {
			r.var=true; 
			Get();
		} else if (la->kind == 26 /* "CONST" */) {
			r.const_=true; 
			Get();
		} else SynErr(102);
		IdentList2(r.identList);
		Expect(36 /* ":" */);
		Type(r.typePtr);
}

void Parser::Qualident(QualidentRecord &r) {
		Ident(r.leftIdent);
		if (StartOf(10)) {
			r.rightIdent=0;
		} else if (la->kind == 48 /* "." */) {
			Get();
			Ident(r.rightIdent);
		} else SynErr(103);
}

void Parser::IdentList2(IdentList2Record &r) {
		Ident(r.ident_);
		if (la->kind == 36 /* ":" */) {
			r.nullOrCommaIdentList=0; 
		} else if (la->kind == 23 /* "," */) {
			r.nullOrCommaIdentList=new IdentList2Record(); abortIfNull(r.nullOrCommaIdentList); 
			Get();
			IdentList2(*(r.nullOrCommaIdentList));
		} else SynErr(104);
}

void Parser::TypeArrayConstExprListMandatory(TypeArrayConstExprListMandatoryRecord &r) {
		ConstExpr(r.dimensionConstExpr);
		if (la->kind == 40 /* "OF" */) {
			r.next=0; 
		} else if (la->kind == 23 /* "," */) {
			r.next=new TypeArrayConstExprListMandatoryRecord(); abortIfNull(r.next); 
			Get();
			TypeArrayConstExprListMandatory(*(r.next));
		} else SynErr(105);
}

void Parser::TypeArray(TypeARRAY &r) {
		if (la->kind == 40 /* "OF" */) {
			r.dimensionsConstExprsListPtr=0; 
		} else if (StartOf(11)) {
			r.dimensionsConstExprsListPtr=new TypeArrayConstExprListMandatoryRecord(); abortIfNull(r.dimensionsConstExprsListPtr); 
			TypeArrayConstExprListMandatory(*(r.dimensionsConstExprsListPtr));
		} else SynErr(106);
		Expect(40 /* "OF" */);
		Type(r.arrayElementTypePtr);
}

void Parser::MandatoryFieldsList(MandatoryFieldsListRecord &r) {
		FieldList(r.recordFieldsList);
		if (la->kind == 34 /* "END" */) {
			r.next=0; 
		} else if (la->kind == 25 /* ";" */) {
			r.next=new MandatoryFieldsListRecord(); abortIfNull(r.next); 
			Get();
			MandatoryFieldsList(*(r.next));
		} else SynErr(107);
}

void Parser::FieldList(FieldListRecord &r) {
		if (la->kind == 25 /* ";" */ || la->kind == 34 /* "END" */) {
			r.fieldsPresent=false; 
		} else if (la->kind == _ident) {
			r.fieldsPresent=true; 
			IdentList(r.identList);
			Expect(36 /* ":" */);
			Type(r.typePtr);
		} else SynErr(108);
}

void Parser::TypeRecord_(TypeRECORD &r) {
		if (la->kind == _ident || la->kind == 25 /* ";" */ || la->kind == 34 /* "END" */) {
			r.optionalQualidentPtr=0; 
		} else if (la->kind == 38 /* "(" */) {
			r.optionalQualidentPtr=new QualidentRecord(); abortIfNull(r.optionalQualidentPtr); 
			Get();
			Qualident(*(r.optionalQualidentPtr));
			Expect(39 /* ")" */);
		} else SynErr(109);
		MandatoryFieldsList(r.fieldsList);
}

void Parser::TypeProcedure(TypePROCEDURE &r) {
		if (la->kind == 25 /* ";" */ || la->kind == 34 /* "END" */ || la->kind == 39 /* ")" */) {
			r.optionalFormalParsPtr=0; 
		} else if (la->kind == 38 /* "(" */) {
			r.optionalFormalParsPtr=new Parser::FormalParsRecord(); abortIfNull(r.optionalFormalParsPtr); 
			FormalPars(*(r.optionalFormalParsPtr));
		} else SynErr(110);
}

void Parser::Statement(StatementRecord*&ptrToStmtRecord) {
		Stmt_EXPR_OR_ASSIGN *psea; Stmt_IF *pif; Stmt_CASE *pcs;
		Stmt_WHILE *pw; Stmt_REPEAT *prpt; Stmt_FOR *pf; Stmt_LOOP *ploop; 
		Stmt_WITH *pwith; Stmt_RETURN *preturn; 
		
		switch (la->kind) {
		case 25 /* ";" */: case 34 /* "END" */: case 53 /* "ELSIF" */: case 55 /* "|" */: case 58 /* "ELSE" */: case 62 /* "UNTIL" */: {
			ptrToStmtRecord=new Stmt_EmptyStmt(); abortIfNull(ptrToStmtRecord); 
			break;
		}
		case _ident: case _string: case _integer: case _real: case 14 /* "+" */: case 15 /* "-" */: case 38 /* "(" */: case 46 /* "NIL" */: case 47 /* "~" */: case 51 /* "{" */: {
			ptrToStmtRecord=psea=new Stmt_EXPR_OR_ASSIGN(); abortIfNull(ptrToStmtRecord); 
			Expr((*psea).lhsExpr);
			if (StartOf(12)) {
				psea->assignment=false; 
			} else if (la->kind == 22 /* ":=" */) {
				psea->assignment=true; 
				Get();
				Expr((*psea).rhsExpr);
			} else SynErr(111);
			break;
		}
		case 57 /* "IF" */: {
			ptrToStmtRecord=pif=new Stmt_IF(); abortIfNull(ptrToStmtRecord); 
			Get();
			Expr((*pif).expr);
			Expect(54 /* "THEN" */);
			StatementSeq((*pif).thenStmtSeq);
			if (la->kind == 34 /* "END" */ || la->kind == 58 /* "ELSE" */) {
				pif->optionalElsifsListPtr=0; 
			} else if (la->kind == 53 /* "ELSIF" */) {
				pif->optionalElsifsListPtr=new MandatoryELSIFsListRecord(); abortIfNull(pif->optionalElsifsListPtr); 
				MandatoryELSIFsList(*((*pif).optionalElsifsListPtr));
			} else SynErr(112);
			if (la->kind == 34 /* "END" */) {
				pif->optionalElsePtr=0; 
			} else if (la->kind == 58 /* "ELSE" */) {
				pif->optionalElsePtr=new StatementSeqRecord(); abortIfNull(pif->optionalElsePtr); 
				Get();
				StatementSeq((*pif).optionalElsePtr);
			} else SynErr(113);
			Expect(34 /* "END" */);
			break;
		}
		case 59 /* "CASE" */: {
			ptrToStmtRecord=pcs=new Stmt_CASE(); abortIfNull(ptrToStmtRecord); 
			Get();
			Expr((*pcs).expr);
			Expect(40 /* "OF" */);
			Case((*pcs).caseFirst);
			if (la->kind == 34 /* "END" */ || la->kind == 58 /* "ELSE" */) {
				(*pcs).optionalOtherCasesPtr=0; 
			} else if (la->kind == 55 /* "|" */) {
				(*pcs).optionalOtherCasesPtr=new CasesRecord(); abortIfNull((*pcs).optionalOtherCasesPtr); 
				Cases(*((*pcs).optionalOtherCasesPtr));
			} else SynErr(114);
			if (la->kind == 34 /* "END" */) {
				(*pcs).optionalElsePtr=0; 
			} else if (la->kind == 58 /* "ELSE" */) {
				(*pcs).optionalElsePtr=new StatementSeqRecord(); abortIfNull((*pcs).optionalElsePtr); 
				Get();
				StatementSeq((*pcs).optionalElsePtr);
			} else SynErr(115);
			Expect(34 /* "END" */);
			break;
		}
		case 60 /* "WHILE" */: {
			ptrToStmtRecord=pw=new Stmt_WHILE(); abortIfNull(ptrToStmtRecord); 
			Get();
			Expr((*pw).expr);
			Expect(56 /* "DO" */);
			StatementSeq((*pw).whileBodyStatementSeq);
			Expect(34 /* "END" */);
			break;
		}
		case 61 /* "REPEAT" */: {
			ptrToStmtRecord=prpt=new Stmt_REPEAT(); abortIfNull(ptrToStmtRecord); 
			Get();
			StatementSeq((*prpt).repeatBodyStatementSeq);
			Expect(62 /* "UNTIL" */);
			Expr((*prpt).expr);
			break;
		}
		case 63 /* "FOR" */: {
			ptrToStmtRecord=pf=new Stmt_FOR(); abortIfNull(ptrToStmtRecord); 
			Get();
			Ident((*pf).forCounterVariableName);
			Expect(22 /* ":=" */);
			Expr((*pf).forCounterVariableInitialValueExpr);
			Expect(44 /* "TO" */);
			Expr((*pf).forCounterVariableToValueExpr);
			if (la->kind == 56 /* "DO" */) {
				(*pf).bySpecified=false; 
			} else if (la->kind == 64 /* "BY" */) {
				(*pf).bySpecified=true; 
				Get();
				ConstExpr((*pf).byValueConstExpr);
			} else SynErr(116);
			Expect(56 /* "DO" */);
			StatementSeq((*pf).forStatementSeq);
			Expect(34 /* "END" */);
			break;
		}
		case 65 /* "LOOP" */: {
			ptrToStmtRecord=ploop=new Stmt_LOOP(); abortIfNull(ptrToStmtRecord); 
			Get();
			StatementSeq((*ploop).loopStatementSeq);
			Expect(34 /* "END" */);
			break;
		}
		case 66 /* "WITH" */: {
			ptrToStmtRecord=pwith=new Stmt_WITH(); abortIfNull(ptrToStmtRecord); 
			Get();
			Guard((*pwith).firstGuard);
			Expect(56 /* "DO" */);
			StatementSeq((*pwith).firstStatementSeq);
			if (la->kind == 34 /* "END" */ || la->kind == 58 /* "ELSE" */) {
				(*pwith).optionalFurtherWithClausesPtr=0; 
			} else if (la->kind == 55 /* "|" */) {
				(*pwith).optionalFurtherWithClausesPtr=new FurtherWithClausesRecord(); abortIfNull((*pwith).optionalFurtherWithClausesPtr); 
				FurtherWithClauses(*((*pwith).optionalFurtherWithClausesPtr));
			} else SynErr(117);
			if (la->kind == 34 /* "END" */) {
				(*pwith).optionalElsePtr=0; 
			} else if (la->kind == 58 /* "ELSE" */) {
				(*pwith).optionalElsePtr=new StatementSeqRecord(); abortIfNull((*pwith).optionalElsePtr); 
				Get();
				StatementSeq((*pwith).optionalElsePtr);
			} else SynErr(118);
			Expect(34 /* "END" */);
			break;
		}
		case 67 /* "EXIT" */: {
			ptrToStmtRecord=new Stmt_EXIT(); abortIfNull(ptrToStmtRecord); 
			Get();
			break;
		}
		case 68 /* "RETURN" */: {
			ptrToStmtRecord=preturn=new Stmt_RETURN(); abortIfNull(ptrToStmtRecord); 
			Get();
			if (StartOf(12)) {
				preturn->exprPresent=false; 
			} else if (StartOf(11)) {
				preturn->exprPresent=true; 
				Expr((*preturn).expr);
			} else SynErr(119);
			break;
		}
		default: SynErr(120); break;
		}
}

void Parser::CaseLabelsLists(CaseLabelsListsRecord &r) {
		CaseLabels(r.caseLabelsNth);
		if (la->kind == 36 /* ":" */) {
			r.optionalFurtherCaseLabelsListsPtr=0; 
		} else if (la->kind == 23 /* "," */) {
			r.optionalFurtherCaseLabelsListsPtr=new CaseLabelsListsRecord(); abortIfNull(r.optionalFurtherCaseLabelsListsPtr); 
			Get();
			CaseLabelsLists(*(r.optionalFurtherCaseLabelsListsPtr));
		} else SynErr(121);
}

void Parser::CaseLabels(CaseLabelsRecord &r) {
		ConstExpr(r.constExpr1);
		if (la->kind == 23 /* "," */ || la->kind == 36 /* ":" */) {
			r.secondConstExprPresent=false; 
		} else if (la->kind == 45 /* ".." */) {
			r.secondConstExprPresent=true; 
			Get();
			ConstExpr(r.constExpr2);
		} else SynErr(122);
}

void Parser::Case(CaseRecord &r) {
		if (la->kind == 34 /* "END" */ || la->kind == 55 /* "|" */ || la->kind == 58 /* "ELSE" */) {
			r.emptyCase=true; 
		} else if (StartOf(11)) {
			r.emptyCase=false; 
			CaseLabelsLists(r.caseLabelsLists);
			Expect(36 /* ":" */);
			StatementSeq(r.stmtSeq);
		} else SynErr(123);
}

void Parser::Guard(GuardRecord &r) {
		Qualident(r.qualident1);
		Expect(36 /* ":" */);
		Qualident(r.qualident2);
}

void Parser::SimpleExprAddOpClause(SimpleExprAddOpRecord &r) {
		AddOp(r.addOp);
		Term(r.term);
		if (StartOf(5)) {
			r.nullOrNextSimpleExprAddOpRecord=0; 
		} else if (la->kind == 14 /* "+" */ || la->kind == 15 /* "-" */ || la->kind == 16 /* "OR" */) {
			r.nullOrNextSimpleExprAddOpRecord=new SimpleExprAddOpRecord(); abortIfNull(r.nullOrNextSimpleExprAddOpRecord); 
			SimpleExprAddOpClause(*r.nullOrNextSimpleExprAddOpRecord);
		} else SynErr(124);
}

void Parser::Term(TermRecord &t) {
		t.factorPtr=0; 
		Factor(t.factorPtr);
		if (StartOf(13)) {
			t.nullOrNextTermMulOpRecord=0; 
		} else if (StartOf(14)) {
			t.nullOrNextTermMulOpRecord=new TermMulOpRecord(); abortIfNull(t.nullOrNextTermMulOpRecord); 
			TermMulOpClause(*t.nullOrNextTermMulOpRecord);
		} else SynErr(125);
}

void Parser::TermMulOpClause(TermMulOpRecord &r) {
		MulOp(r.mulOp);
		r.factorPtr=0; 
		Factor(r.factorPtr);
		if (StartOf(13)) {
			r.nullOrNextTermMulOpRecord=0; 
		} else if (StartOf(14)) {
			r.nullOrNextTermMulOpRecord=new TermMulOpRecord(); abortIfNull(r.nullOrNextTermMulOpRecord); 
			TermMulOpClause(*r.nullOrNextTermMulOpRecord);
		} else SynErr(126);
}

void Parser::Factor(FactorRecord *&factorPtr) {
		FactorRecord_DesignatorMaybeWithExprList *fd;
		FactorRecord_numberLiteral *fn;
		FactorRecord_character *fc;
		FactorRecord_string *fs;
		FactorRecord_Set *fset;
		FactorRecord_Expr *fexpr;
		FactorRecord_tildeFactor *ftf;
		
		if (la->kind == _ident) {
			factorPtr=fd=new FactorRecord_DesignatorMaybeWithExprList(); abortIfNull(factorPtr); 
			DesignatorMaybeWithExprList((*fd).r);
		} else if (la->kind == _integer || la->kind == _real) {
			factorPtr=fn=new FactorRecord_numberLiteral(); abortIfNull(factorPtr); 
			number((*fn).numLiteral);
		} else if (la->kind == _string) {
			factorPtr=fc=new FactorRecord_character(); abortIfNull(factorPtr); 
			Character((*fc).ch);
		} else if (la->kind == _string) {
			factorPtr=fs=new FactorRecord_string(); abortIfNull(factorPtr); 
			String((*fs).s);
		} else if (la->kind == 46 /* "NIL" */) {
			factorPtr=new FactorRecord_NIL(); abortIfNull(factorPtr); 
			Get();
		} else if (la->kind == 51 /* "{" */) {
			factorPtr=fset=new FactorRecord_Set(); abortIfNull(factorPtr); 
			Set((*fset).set);
		} else if (la->kind == 38 /* "(" */) {
			factorPtr=fexpr=new FactorRecord_Expr(); abortIfNull(factorPtr); 
			Get();
			Expr((*fexpr).expr);
			Expect(39 /* ")" */);
		} else if (la->kind == 47 /* "~" */) {
			factorPtr=ftf=new FactorRecord_tildeFactor(); abortIfNull(factorPtr); 
			ftf->factorPtr=0; 
			Get();
			Factor((*ftf).factorPtr);
		} else SynErr(127);
}

void Parser::DesignatorMaybeWithExprList(DesignatorMaybeWithExprListRecord &r) {
		IdentRec(r.identRec.ident_);
		if (StartOf(15)) {
			r.nullOrPtrToNextDesignatorMaybeWithExprListRepeatingPartRecord=0; 
		} else if (StartOf(16)) {
			DesignatorMaybeWithExprListRepeatingPartClause(r.nullOrPtrToNextDesignatorMaybeWithExprListRepeatingPartRecord);
		} else SynErr(128);
}

void Parser::Character(wchar_t* &tok) {
		character();
		tok = coco_string_create(t->val); 
}

void Parser::String(wchar_t* &tok) {
		Expect(_string);
		tok = coco_string_create(t->val); 
}

void Parser::Set(SetRecord &r) {
		r.nullOrPtrToNextSet=0; 
		Expect(51 /* "{" */);
		if (la->kind == 52 /* "}" */) {
			r.emptySet=true; 
		} else if (StartOf(11)) {
			r.emptySet=false; 
			Element(r.range);
			if (la->kind == 52 /* "}" */) {
			} else if (la->kind == 23 /* "," */) {
				r.nullOrPtrToNextSet=new SetRecord(); abortIfNull(r.nullOrPtrToNextSet); 
				ElementRangeList(*(r.nullOrPtrToNextSet));
			} else SynErr(129);
		} else SynErr(130);
		Expect(52 /* "}" */);
}

void Parser::DesignatorMaybeWithExprListRepeatingPartClause(DesignatorMaybeWithExprListRepeatingPartRecord *&r) {
		r=0; 
		if (la->kind == 48 /* "." */) {
			DesignatorMaybeWithExprListRepeatingPartRecordCL1 *rr=new DesignatorMaybeWithExprListRepeatingPartRecordCL1(); abortIfNull(rr); r=rr; 
			Get();
			IdentRec((*rr).clause1_identRec);
		} else if (la->kind == 49 /* "[" */) {
			DesignatorMaybeWithExprListRepeatingPartRecordCL2 *rr=new DesignatorMaybeWithExprListRepeatingPartRecordCL2(); abortIfNull(rr); r=rr; 
			Get();
			ExprList((*rr).clause2_exprList);
			Expect(50 /* "]" */);
		} else if (la->kind == 37 /* "^" */) {
			DesignatorMaybeWithExprListRepeatingPartRecordCL3 *rr=new DesignatorMaybeWithExprListRepeatingPartRecordCL3(); abortIfNull(rr); r=rr; 
			Get();
		} else if (la->kind == 38 /* "(" */) {
			DesignatorMaybeWithExprListRepeatingPartRecordCL4 *rr=new DesignatorMaybeWithExprListRepeatingPartRecordCL4(); abortIfNull(rr); r=rr; 
			Get();
			QualidentOrOptionalExprList((*rr).clause4_qualidentOrOptionalExprList);
			Expect(39 /* ")" */);
		} else SynErr(131);
		if(r!=0){ 
		if (StartOf(15)) {
			r->nullOrPtrToNextDesignatorMaybeWithExprListRepeatingPartRecord=0; 
		} else if (StartOf(16)) {
			DesignatorMaybeWithExprListRepeatingPartClause((*r).nullOrPtrToNextDesignatorMaybeWithExprListRepeatingPartRecord);
		} else SynErr(132);
		} 
}

void Parser::IdentRec(wchar_t* &tok) {
		Ident(tok);
}

void Parser::ExprList(ExprListRecord &r) {
		Expr(r.expr);
		if (la->kind == 39 /* ")" */ || la->kind == 50 /* "]" */) {
			r.nullOrCommaExprList=0; 
		} else if (la->kind == 23 /* "," */) {
			r.nullOrCommaExprList=new ExprListRecord(); abortIfNull(r.nullOrCommaExprList); 
			Get();
			ExprList(*(r.nullOrCommaExprList));
		} else SynErr(133);
}

void Parser::QualidentOrOptionalExprList(QualidentOrOptionalExprListRecord &r) {
		if (la->kind == 39 /* ")" */) {
			r.exprListPresent=false; 
		} else if (StartOf(11)) {
			r.exprListPresent=true; 
			ExprList(r.exprList);
		} else SynErr(134);
}

void Parser::ElementRangeList(SetRecord &r) {
		r.emptySet=false; r.nullOrPtrToNextSet=0; 
		Expect(23 /* "," */);
		Element(r.range);
		if (la->kind == 52 /* "}" */) {
			r.nullOrPtrToNextSet=0; 
		} else if (la->kind == 23 /* "," */) {
			r.nullOrPtrToNextSet=new SetRecord(); abortIfNull(r.nullOrPtrToNextSet); 
			ElementRangeList(*(r.nullOrPtrToNextSet));
		} else SynErr(135);
}

void Parser::Element(ElementRangeRecord &r) {
		Expr(r.expr1);
		if (la->kind == 23 /* "," */ || la->kind == 52 /* "}" */) {
			r.isrange=false; 
		} else if (la->kind == 45 /* ".." */) {
			r.isrange=true; 
			Get();
			Expr(r.expr2);
		} else SynErr(136);
}

void Parser::MandatoryELSIFsList(MandatoryELSIFsListRecord &r) {
		Expect(53 /* "ELSIF" */);
		Expr(r.expr);
		Expect(54 /* "THEN" */);
		StatementSeq(r.thenStmtSeq);
		if (la->kind == 34 /* "END" */ || la->kind == 58 /* "ELSE" */) {
			r.optionalElsifsListPtr=0; 
		} else if (la->kind == 53 /* "ELSIF" */) {
			r.optionalElsifsListPtr=new MandatoryELSIFsListRecord(); abortIfNull(r.optionalElsifsListPtr); 
			MandatoryELSIFsList(*(r.optionalElsifsListPtr));
		} else SynErr(137);
}

void Parser::Cases(CasesRecord &r) {
		Expect(55 /* "|" */);
		Case(r.caseNth);
		if (la->kind == 34 /* "END" */ || la->kind == 58 /* "ELSE" */) {
			r.optionalOtherCasesPtr=0; 
		} else if (la->kind == 55 /* "|" */) {
			r.optionalOtherCasesPtr=new CasesRecord(); abortIfNull(r.optionalOtherCasesPtr); 
			Cases(*(r.optionalOtherCasesPtr));
		} else SynErr(138);
}

void Parser::FurtherWithClauses(FurtherWithClausesRecord &r) {
		Expect(55 /* "|" */);
		Guard(r.guard);
		Expect(56 /* "DO" */);
		StatementSeq(r.statementSeq);
		if (la->kind == 34 /* "END" */ || la->kind == 58 /* "ELSE" */) {
			r.next=0; 
		} else if (la->kind == 55 /* "|" */) {
			r.next=new FurtherWithClausesRecord(); abortIfNull(r.next); 
			FurtherWithClauses(*(r.next));
		} else SynErr(139);
}

void Parser::Module(ModuleRecord &r) {
		wchar_t *endName; 
		Expect(31 /* "MODULE" */);
		Ident(r.moduleName);
		Expect(25 /* ";" */);
		if (StartOf(17)) {
			r.importListPtr=0; 
		} else if (la->kind == 24 /* "IMPORT" */) {
			r.importListPtr=new ImportListRecord(); abortIfNull(r.importListPtr); 
			ImportList(*(r.importListPtr));
		} else SynErr(140);
		DeclSeq(r.declSeq);
		r.stmtSeq=0; 
		Expect(34 /* "END" */);
		Ident(endName);
		if(!coco_string_equal(endName, r.moduleName)){
		Err(L"MODULE ident ... END ident.: idents do not match.");
		} 
		
		Expect(48 /* "." */);
		wprintf(L"MODULE %ls\n",r.moduleName); 
}

void Parser::Integer(wchar_t* &tok) {
		Expect(_integer);
		tok = coco_string_create(t->val); 
}

void Parser::Real(wchar_t* &tok) {
		Expect(_real);
		tok = coco_string_create(t->val); 
}

void Parser::Oberon() {
		InitDeclarations(); modulePtr=new ModuleRecord(); abortIfNull(modulePtr); 
		Module(*(modulePtr));
}




// If the user declared a method Init and a mehtod Destroy they should
// be called in the contructur and the destructor respctively.
//
// The following templates are used to recognize if the user declared
// the methods Init and Destroy.

template<typename T>
struct ParserInitExistsRecognizer {
	template<typename U, void (U::*)() = &U::Init>
	struct ExistsIfInitIsDefinedMarker{};

	struct InitIsMissingType {
		char dummy1;
	};
	
	struct InitExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static InitIsMissingType is_here(...);

	// exist only if ExistsIfInitIsDefinedMarker is defined
	template<typename U>
	static InitExistsType is_here(ExistsIfInitIsDefinedMarker<U>*);

	enum { InitExists = (sizeof(is_here<T>(NULL)) == sizeof(InitExistsType)) };
};

template<typename T>
struct ParserDestroyExistsRecognizer {
	template<typename U, void (U::*)() = &U::Destroy>
	struct ExistsIfDestroyIsDefinedMarker{};

	struct DestroyIsMissingType {
		char dummy1;
	};
	
	struct DestroyExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static DestroyIsMissingType is_here(...);

	// exist only if ExistsIfDestroyIsDefinedMarker is defined
	template<typename U>
	static DestroyExistsType is_here(ExistsIfDestroyIsDefinedMarker<U>*);

	enum { DestroyExists = (sizeof(is_here<T>(NULL)) == sizeof(DestroyExistsType)) };
};

// The folloing templates are used to call the Init and Destroy methods if they exist.

// Generic case of the ParserInitCaller, gets used if the Init method is missing
template<typename T, bool = ParserInitExistsRecognizer<T>::InitExists>
struct ParserInitCaller {
	static void CallInit(T *t) {
		// nothing to do
	}
};

// True case of the ParserInitCaller, gets used if the Init method exists
template<typename T>
struct ParserInitCaller<T, true> {
	static void CallInit(T *t) {
		t->Init();
	}
};

// Generic case of the ParserDestroyCaller, gets used if the Destroy method is missing
template<typename T, bool = ParserDestroyExistsRecognizer<T>::DestroyExists>
struct ParserDestroyCaller {
	static void CallDestroy(T *t) {
		// nothing to do
	}
};

// True case of the ParserDestroyCaller, gets used if the Destroy method exists
template<typename T>
struct ParserDestroyCaller<T, true> {
	static void CallDestroy(T *t) {
		t->Destroy();
	}
};

void Parser::Parse() {
	t = NULL;
	la = dummyToken = new Token();
	la->val = coco_string_create(L"Dummy Token");
	Get();
	Oberon();

}

Parser::Parser(Scanner *scanner, Errors* errors_) {
	//parserListener=0;
	maxT = 69;

	ParserInitCaller<Parser>::CallInit(this);
	dummyToken = NULL;
	t = la = NULL;
	minErrDist = 2;
	errDist = minErrDist;
	this->scanner = scanner;
	errors = errors_;
}

bool Parser::StartOf(int s) {
	const bool T = true;
	const bool x = false;

	static bool set[18][71] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,T,T, T,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, x,T,x,x, x,x,x,x, x,x,T,x, T,x,x,T, T,x,x,x, T,T,x,x, x,x,T,x, T,T,T,T, T,x,T,x, x,x,T,x, T,x,x,x, x,x,x},
		{x,T,x,T, T,T,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,T,T, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,T,x,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,T,T, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,T,T, T,T,T,T, T,T,x,x, x,x,x,x, x,x,T,T, x,T,x,x, x,x,x,x, x,x,T,x, T,x,x,T, T,x,x,x, T,T,x,x, x,x,T,x, T,T,T,T, T,x,T,x, x,x,T,x, T,x,x,x, x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,T,x, x,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,x, x,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,T, x,x,T,x, x,x,T,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, x,T,x,x, x,x,x,x, x,x,x,x, T,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,T,x, T,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,T,x,T, T,T,x,x, x,x,x,x, x,x,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,T,T, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,T, x,x,T,x, x,x,T,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,T,T, T,T,T,T, T,T,T,T, T,x,x,x, x,x,T,T, x,T,x,x, x,x,x,x, x,x,T,x, T,x,x,T, T,x,x,x, T,T,x,x, x,x,T,x, T,T,T,T, T,x,T,x, x,x,T,x, T,x,x,x, x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,T,T, T,T,T,T, T,T,T,T, T,T,T,T, T,T,T,T, x,T,x,x, x,x,x,x, x,x,T,x, T,x,x,T, T,x,x,x, T,T,x,x, x,x,T,x, T,T,T,T, T,x,T,x, x,x,T,x, T,x,x,x, x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,T,x, x,x,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x}
	};



	return set[s][la->kind];
}

Parser::~Parser() {
	ParserDestroyCaller<Parser>::CallDestroy(this);
	//delete errors;
	delete dummyToken;
}

Errors::Errors() {
	count = 0;
}

void Errors::SynErr(int line, int col, int n) {
	wchar_t* s;
	switch (n) {
			case 0: s = coco_string_create(L"EOF expected"); break;
			case 1: s = coco_string_create(L"ident expected"); break;
			case 2: s = coco_string_create(L"badString expected"); break;
			case 3: s = coco_string_create(L"string expected"); break;
			case 4: s = coco_string_create(L"integer expected"); break;
			case 5: s = coco_string_create(L"real expected"); break;
			case 6: s = coco_string_create(L"\"=\" expected"); break;
			case 7: s = coco_string_create(L"\"#\" expected"); break;
			case 8: s = coco_string_create(L"\"<\" expected"); break;
			case 9: s = coco_string_create(L"\"<=\" expected"); break;
			case 10: s = coco_string_create(L"\">\" expected"); break;
			case 11: s = coco_string_create(L"\">=\" expected"); break;
			case 12: s = coco_string_create(L"\"IN\" expected"); break;
			case 13: s = coco_string_create(L"\"IS\" expected"); break;
			case 14: s = coco_string_create(L"\"+\" expected"); break;
			case 15: s = coco_string_create(L"\"-\" expected"); break;
			case 16: s = coco_string_create(L"\"OR\" expected"); break;
			case 17: s = coco_string_create(L"\"*\" expected"); break;
			case 18: s = coco_string_create(L"\"/\" expected"); break;
			case 19: s = coco_string_create(L"\"DIV\" expected"); break;
			case 20: s = coco_string_create(L"\"MOD\" expected"); break;
			case 21: s = coco_string_create(L"\"&\" expected"); break;
			case 22: s = coco_string_create(L"\":=\" expected"); break;
			case 23: s = coco_string_create(L"\",\" expected"); break;
			case 24: s = coco_string_create(L"\"IMPORT\" expected"); break;
			case 25: s = coco_string_create(L"\";\" expected"); break;
			case 26: s = coco_string_create(L"\"CONST\" expected"); break;
			case 27: s = coco_string_create(L"\"TYPE\" expected"); break;
			case 28: s = coco_string_create(L"\"VAR\" expected"); break;
			case 29: s = coco_string_create(L"\"PROCEDURE\" expected"); break;
			case 30: s = coco_string_create(L"\"ON\" expected"); break;
			case 31: s = coco_string_create(L"\"MODULE\" expected"); break;
			case 32: s = coco_string_create(L"\"LOAD\" expected"); break;
			case 33: s = coco_string_create(L"\"BEGIN\" expected"); break;
			case 34: s = coco_string_create(L"\"END\" expected"); break;
			case 35: s = coco_string_create(L"\"UNLOAD\" expected"); break;
			case 36: s = coco_string_create(L"\":\" expected"); break;
			case 37: s = coco_string_create(L"\"^\" expected"); break;
			case 38: s = coco_string_create(L"\"(\" expected"); break;
			case 39: s = coco_string_create(L"\")\" expected"); break;
			case 40: s = coco_string_create(L"\"OF\" expected"); break;
			case 41: s = coco_string_create(L"\"ARRAY\" expected"); break;
			case 42: s = coco_string_create(L"\"RECORD\" expected"); break;
			case 43: s = coco_string_create(L"\"POINTER\" expected"); break;
			case 44: s = coco_string_create(L"\"TO\" expected"); break;
			case 45: s = coco_string_create(L"\"..\" expected"); break;
			case 46: s = coco_string_create(L"\"NIL\" expected"); break;
			case 47: s = coco_string_create(L"\"~\" expected"); break;
			case 48: s = coco_string_create(L"\".\" expected"); break;
			case 49: s = coco_string_create(L"\"[\" expected"); break;
			case 50: s = coco_string_create(L"\"]\" expected"); break;
			case 51: s = coco_string_create(L"\"{\" expected"); break;
			case 52: s = coco_string_create(L"\"}\" expected"); break;
			case 53: s = coco_string_create(L"\"ELSIF\" expected"); break;
			case 54: s = coco_string_create(L"\"THEN\" expected"); break;
			case 55: s = coco_string_create(L"\"|\" expected"); break;
			case 56: s = coco_string_create(L"\"DO\" expected"); break;
			case 57: s = coco_string_create(L"\"IF\" expected"); break;
			case 58: s = coco_string_create(L"\"ELSE\" expected"); break;
			case 59: s = coco_string_create(L"\"CASE\" expected"); break;
			case 60: s = coco_string_create(L"\"WHILE\" expected"); break;
			case 61: s = coco_string_create(L"\"REPEAT\" expected"); break;
			case 62: s = coco_string_create(L"\"UNTIL\" expected"); break;
			case 63: s = coco_string_create(L"\"FOR\" expected"); break;
			case 64: s = coco_string_create(L"\"BY\" expected"); break;
			case 65: s = coco_string_create(L"\"LOOP\" expected"); break;
			case 66: s = coco_string_create(L"\"WITH\" expected"); break;
			case 67: s = coco_string_create(L"\"EXIT\" expected"); break;
			case 68: s = coco_string_create(L"\"RETURN\" expected"); break;
			case 69: s = coco_string_create(L"??? expected"); break;
			case 70: s = coco_string_create(L"invalid number"); break;
			case 71: s = coco_string_create(L"invalid Relation"); break;
			case 72: s = coco_string_create(L"invalid AddOp"); break;
			case 73: s = coco_string_create(L"invalid MulOp"); break;
			case 74: s = coco_string_create(L"invalid Expr"); break;
			case 75: s = coco_string_create(L"invalid SimpleExpr"); break;
			case 76: s = coco_string_create(L"invalid SimpleExpr"); break;
			case 77: s = coco_string_create(L"invalid ModuleImportEntry"); break;
			case 78: s = coco_string_create(L"invalid ModuleImportEntryList"); break;
			case 79: s = coco_string_create(L"invalid DeclSeqConstDeclListMandatory"); break;
			case 80: s = coco_string_create(L"invalid DeclSeqConstDeclList"); break;
			case 81: s = coco_string_create(L"invalid DeclSeqTypeDeclListMandatory"); break;
			case 82: s = coco_string_create(L"invalid DeclSeqTypeDeclList"); break;
			case 83: s = coco_string_create(L"invalid DeclSeqVarDeclListMandatory"); break;
			case 84: s = coco_string_create(L"invalid DeclSeqVarDeclList"); break;
			case 85: s = coco_string_create(L"invalid DeclSeqConstTypeVarListMandatory"); break;
			case 86: s = coco_string_create(L"invalid DeclSeqConstTypeVarListMandatory"); break;
			case 87: s = coco_string_create(L"invalid DeclSeqConstTypeVarList"); break;
			case 88: s = coco_string_create(L"invalid DeclSeqProcDeclFwdDeclListMandatory"); break;
			case 89: s = coco_string_create(L"invalid DeclSeqProcDeclFwdDeclListMandatory"); break;
			case 90: s = coco_string_create(L"invalid ProcDecl"); break;
			case 91: s = coco_string_create(L"invalid DeclSeqProcDeclFwdDeclList"); break;
			case 92: s = coco_string_create(L"invalid StatementSeq"); break;
			case 93: s = coco_string_create(L"invalid IdentDef"); break;
			case 94: s = coco_string_create(L"invalid Type"); break;
			case 95: s = coco_string_create(L"invalid IdentList"); break;
			case 96: s = coco_string_create(L"invalid OptionalReceiver"); break;
			case 97: s = coco_string_create(L"invalid Receiver"); break;
			case 98: s = coco_string_create(L"invalid OptionalFormalPars"); break;
			case 99: s = coco_string_create(L"invalid FormalPars"); break;
			case 100: s = coco_string_create(L"invalid FormalPars"); break;
			case 101: s = coco_string_create(L"invalid FPSectionsListMandatory"); break;
			case 102: s = coco_string_create(L"invalid FPSection"); break;
			case 103: s = coco_string_create(L"invalid Qualident"); break;
			case 104: s = coco_string_create(L"invalid IdentList2"); break;
			case 105: s = coco_string_create(L"invalid TypeArrayConstExprListMandatory"); break;
			case 106: s = coco_string_create(L"invalid TypeArray"); break;
			case 107: s = coco_string_create(L"invalid MandatoryFieldsList"); break;
			case 108: s = coco_string_create(L"invalid FieldList"); break;
			case 109: s = coco_string_create(L"invalid TypeRecord_"); break;
			case 110: s = coco_string_create(L"invalid TypeProcedure"); break;
			case 111: s = coco_string_create(L"invalid Statement"); break;
			case 112: s = coco_string_create(L"invalid Statement"); break;
			case 113: s = coco_string_create(L"invalid Statement"); break;
			case 114: s = coco_string_create(L"invalid Statement"); break;
			case 115: s = coco_string_create(L"invalid Statement"); break;
			case 116: s = coco_string_create(L"invalid Statement"); break;
			case 117: s = coco_string_create(L"invalid Statement"); break;
			case 118: s = coco_string_create(L"invalid Statement"); break;
			case 119: s = coco_string_create(L"invalid Statement"); break;
			case 120: s = coco_string_create(L"invalid Statement"); break;
			case 121: s = coco_string_create(L"invalid CaseLabelsLists"); break;
			case 122: s = coco_string_create(L"invalid CaseLabels"); break;
			case 123: s = coco_string_create(L"invalid Case"); break;
			case 124: s = coco_string_create(L"invalid SimpleExprAddOpClause"); break;
			case 125: s = coco_string_create(L"invalid Term"); break;
			case 126: s = coco_string_create(L"invalid TermMulOpClause"); break;
			case 127: s = coco_string_create(L"invalid Factor"); break;
			case 128: s = coco_string_create(L"invalid DesignatorMaybeWithExprList"); break;
			case 129: s = coco_string_create(L"invalid Set"); break;
			case 130: s = coco_string_create(L"invalid Set"); break;
			case 131: s = coco_string_create(L"invalid DesignatorMaybeWithExprListRepeatingPartClause"); break;
			case 132: s = coco_string_create(L"invalid DesignatorMaybeWithExprListRepeatingPartClause"); break;
			case 133: s = coco_string_create(L"invalid ExprList"); break;
			case 134: s = coco_string_create(L"invalid QualidentOrOptionalExprList"); break;
			case 135: s = coco_string_create(L"invalid ElementRangeList"); break;
			case 136: s = coco_string_create(L"invalid Element"); break;
			case 137: s = coco_string_create(L"invalid MandatoryELSIFsList"); break;
			case 138: s = coco_string_create(L"invalid Cases"); break;
			case 139: s = coco_string_create(L"invalid FurtherWithClauses"); break;
			case 140: s = coco_string_create(L"invalid Module"); break;

		default:
		{
			wchar_t format[20];
			coco_swprintf(format, 20, L"error %d", n);
			s = coco_string_create(format);
		}
		break;
	}
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	coco_string_delete(s);
	count++;
}

void Errors::Error(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	count++;
}

void Errors::Warning(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
}

void Errors::Warning(const wchar_t *s) {
	wprintf(L"%ls\n", s);
}

void Errors::Exception(const wchar_t* s) {
	wprintf(L"%ls", s); 
	exit(1);
}

Value* literalNumberRecord::calculate_numberValue_from_literal(){
	return new ValueTBD();
}

namespace ModTab{class Module;}

Value* Parser::DesignatorMaybeWithExprListRepeatingPartRecordCL1::calc(Parser* parser, identRecord* id1Ptr, SymbolTable &tab){
			identRecord id2; id2.ident_=clause1_identRec;
			struct Obj* obj = tab.Find(id1Ptr->ident_);
			if (obj==0) {
				tab.Err(L"Object not found");
				tab.Err(id1Ptr->ident_);
				return new ValueOfIdentDotIdent(*id1Ptr, id2);
			}else{
				if(obj->kind==OKscope && obj->type!=0 && obj->type->getTypeNumber()==type_number_MODULE) {
					wchar_t* dealiased=0;
					if(obj->data!=0){
						ModAliasRefDO* DO = (ModAliasRefDO*)(obj->data);
						dealiased=DO->modName;
					}
					else 
						dealiased = id1Ptr->ident_;
					ModTab::Module* m = parser->getmodtab()->Find(dealiased);
					if(m==0){
						tab.Err(L"module not found");tab.Err(id1Ptr->ident_);
						return new ValueOfIdentDotIdent(*id1Ptr, id2);
					}else{
						SymbolTable * tab2 = m->parser->tab;
						Obj* obj2 = tab2->Find(id2.ident_);
						if(obj2->kind==OKproc && obj2->type!=0 && obj2->type->getTypeNumber()==type_number_PROCEDURE &&
						 		((obj2->data) != 0) && obj2->data->getKind()==DeclSeqProcDOK){
								((DeclSeqProcDO*)(obj2->data))->DeclSeqProcPTR->callProcedure(m->parser);
						}
						return new ValueOfIdentDotIdent(*id1Ptr, id2);
					}
				}else{
					if(obj->kind==OKvar) {
						wprintf(L"\nVAR: %ls DOT %ls TYPE: ", id1Ptr->ident_, clause1_identRec); PRINT_TYPE_AS_STRING(obj->type);
						return new ValueOfIdentDotIdent(*id1Ptr, id2);
					}else{
						tab.Err(L"Must be OKMODULE or OKVAR, but is unknown");
						tab.Err(id1Ptr->ident_);
						return new ValueOfIdentDotIdent(*id1Ptr, id2);
					}
				}
		}
}

void Parser::StatementSeqRecord::interpret(Parser*p, CodeGenerator &codegen, SymbolTable &tab){
	codegen.InterpretStmtSeq(p, this, tab);
}

/* } // namespace
 */
