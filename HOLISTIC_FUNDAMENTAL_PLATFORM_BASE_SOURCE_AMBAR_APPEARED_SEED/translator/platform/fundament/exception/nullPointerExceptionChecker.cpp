/*
 * nullPointerExceptionChecker.cpp
 *
 *  Created on: 27 мая 2014 г.
 *      Author: egp
 */

#include "nullPointerExceptionChecker.h"
#include "nullPointerException.h"

void fundament_exceptions::nullPointerExceptionChecker::checkForNullPointer(void* ptr){
	if(ptr==0){
		throw new nullPointerException();
	}
}
