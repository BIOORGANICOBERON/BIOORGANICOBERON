/*
 * InterpreterBackendConstant.h
 *
 *  Created on: 03.01.2013
 *      Author: egp
 */

#ifndef INTERPRETERBACKENDCONSTANT_H_
#define INTERPRETERBACKENDCONSTANT_H_

#include "InterpreterBackendSymbol.h"

class InterpreterBackendConstant: public InterpreterBackendSymbol {
public:
	InterpreterBackendConstant();
	virtual ~InterpreterBackendConstant();
};

#endif /* INTERPRETERBACKENDCONSTANT_H_ */
