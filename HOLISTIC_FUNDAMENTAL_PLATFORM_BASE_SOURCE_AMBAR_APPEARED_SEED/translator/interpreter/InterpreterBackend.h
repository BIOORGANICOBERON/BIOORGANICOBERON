/*
 * InterpreterBackend.h
 *
 *  Created on: 03.01.2013
 *      Author: egp
 */

#ifndef INTERPRETERBACKEND_H_
#define INTERPRETERBACKEND_H_

#include "../parser/AbstractBackend.h"
#include "InterpreterBackendMemory.h"

class InterpreterBackend: public AbstractBackend {
	InterpreterBackendMemory memory;
public:
	InterpreterBackend();
	virtual TYPE getType(){return interpreter;}
	virtual ~InterpreterBackend();
};

#endif /* INTERPRETERBACKEND_H_ */
