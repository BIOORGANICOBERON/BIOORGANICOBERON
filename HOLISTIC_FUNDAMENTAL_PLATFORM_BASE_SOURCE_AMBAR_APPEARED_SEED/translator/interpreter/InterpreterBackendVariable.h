/*
 * InterpreterBackendVariable.h
 *
 *  Created on: 03.01.2013
 *      Author: egp
 */

#ifndef INTERPRETERBACKENDVARIABLE_H_
#define INTERPRETERBACKENDVARIABLE_H_

#include "InterpreterBackendSymbol.h"

class InterpreterBackendVariable: public InterpreterBackendSymbol {
public:
	InterpreterBackendVariable();
	virtual ~InterpreterBackendVariable();
};

#endif /* INTERPRETERBACKENDVARIABLE_H_ */
