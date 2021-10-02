/*
 * nullPointerException.h
 *
 *  Created on: 27 мая 2014 г.
 *      Author: egp
 */

#ifndef NULLPOINTEREXCEPTION_H_
#define NULLPOINTEREXCEPTION_H_

#include "exception.h"

namespace fundament_exceptions {

	class nullPointerException: public exception {
	public:
		nullPointerException();
		virtual void finalize();
	};

}  // namespace fundament_exceptions

#endif /* NULLPOINTEREXCEPTION_H_ */
