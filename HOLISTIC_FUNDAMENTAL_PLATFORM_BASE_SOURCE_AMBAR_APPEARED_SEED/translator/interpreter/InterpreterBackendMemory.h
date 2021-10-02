/*
 * InterpreterBackendMemory.h
 *
 *  Created on: 03.01.2013
 *      Author: egp
 */

#ifndef INTERPRETERBACKENDMEMORY_H_
#define INTERPRETERBACKENDMEMORY_H_

class InterpreterBackendMemory {
public:
	InterpreterBackendMemory();
//	InterpreterBackendSymbol GetExistingSymbol(wchar_t* symbolName){}
	virtual ~InterpreterBackendMemory();
};

#endif /* INTERPRETERBACKENDMEMORY_H_ */
