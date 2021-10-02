#include <assert.h>
#include "Types.h"
#include "Parser.h"

hashmap1Type str2type(0,my_wstr_hash);

hashmap2Type type2str;

Value* TermRecord::calculate(Parser *parser, SymbolTable& tab){
	assert(factorPtr!=0);
	Value* v1 = factorPtr->calculate(parser, tab);
	if(nullOrNextTermMulOpRecord!=0){
		v1 = nullOrNextTermMulOpRecord->calculate(v1, tab);
	}
	return v1;
};

Value* Value::multiply(signed int number){
	return new ValueMultipliedBySignedInt(this, number);
}

void ValueNumber::printToStdout(){
	const wchar_t* numtypestr=L"UNKNOWN_LITERAL_NUM";
	if(numLiteral->literal_type==literal_int)numtypestr=L"LITERAL_INT";
	else
		if(numLiteral->literal_type==literal_real)numtypestr=L"LITERAL_REAL";
	wprintf(L"(value number %ls; type:%ls)",numLiteral->tokenString, numtypestr);
}


TypeVariant TypeQualidentVariants_diagnoseType(const wchar_t * typeName) {
	const wstring name(typeName);
	hashmap1Type::const_iterator it=str2type.find(name);
	TypeVariant tv = it==str2type.end() ? TypeVariant::UNKNOWN_TYPE : (TypeVariant)(it->second);
	bool end = it==str2type.end();
#if 0
	wprintf(L"TypeQualidentVariants_static_init() str2type\n");
	hashmap1Type::const_iterator it1=str2type.begin();
	for (; !(it1==str2type.end()); ++it1) {
		const wstring& x=it1->first;
		const wchar_t* s=x.c_str();
		wprintf(L"'%ls' %ld\n",s, it1->second);
	}
	wprintf(L"TypeQualidentVariants_static_init() type2str\n");
	hashmap2Type::const_iterator it2=type2str.begin();
	for (; !(it2==type2str.end()); ++it2) {
		const wstring& x=it2->second;
		const wchar_t* s=x.c_str();
		wprintf(L"'%ls' %ld\n",s, it2->first);
	}
	wprintf(L"TypeQualidentVariants_diagnoseType sz1=%d sz2=%d\n", type2str.size(), str2type.size());
	wprintf(L"TypeQualidentVariants_diagnoseType '%ls' len=%d tv=%d end=%d\n",name.c_str(), name.length(), (int)tv, (int)end);
	exit(1);
#else
	return tv;
#endif
}

wstring TypeVariant2wstr(TypeVariant ty){
	if (ty==TypeVariant::UNKNOWN_TYPE)return L"(UNK_TYPE)";
	hashmap2Type::const_iterator it=type2str.find((int)ty);
	if(it==type2str.end()){
		wstring tostr;
		basic_ostringstream<wchar_t> ostr(tostr);
		ostr<<L"(UNK_TYPE, val="<<(int)ty<<L")";
		return ostr.str();
	}else return it->second;
}

void TypeQualidentVariants_static_init(){
	TypeQualidentVariants_init_add(L"",TypeVariant::UNKNOWN_TYPE);
	TypeQualidentVariants_init_add(L"LONGINT",TypeVariant::LONGINT);
#if 0
	wprintf(L"TypeQualidentVariants_static_init() str2type\n");
	hashmap1Type::const_iterator it1=str2type.begin();
	for (; !(it1==str2type.end()); ++it1) {
		const wstring& x=it1->first;
		const wchar_t* s=x.c_str();
		wprintf(L"'%ls' %ld\n",s, it1->second);
	}
	wprintf(L"TypeQualidentVariants_static_init() type2str\n");
	hashmap2Type::const_iterator it2=type2str.begin();
	for (; !(it2==type2str.end()); ++it2) {
		const wstring& x=it2->second;
		const wchar_t* s=x.c_str();
		wprintf(L"'%ls' %ld\n",s, it2->first);
	}
	const wstring* x = new wstring(L"LONGINT");
	TypeQualidentVariants_diagnoseType(x->c_str());
	exit(1);
#endif
}

void TypeQualidentVariants_init_add(wstring name, TypeVariant ty){
	str2type[name]=(int)ty;
	type2str[(int)ty]=name;
}

