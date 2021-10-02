/*
 * exception.cpp
 *
 *  Created on: 27 мая 2014 г.
 *      Author: egp
 */

#include "exception.h"
#include "assert.h"

using namespace fundament_exceptions;

exception::exception(std::wstring* message_) {
	assert(message_!=0);
	message=message_;
}

exception::exception() {
	message=new std::wstring(L"");
	assert(message!=0);
}

void exception::finalize() {
	delete message; message=0;
	entity::finalize();
}

