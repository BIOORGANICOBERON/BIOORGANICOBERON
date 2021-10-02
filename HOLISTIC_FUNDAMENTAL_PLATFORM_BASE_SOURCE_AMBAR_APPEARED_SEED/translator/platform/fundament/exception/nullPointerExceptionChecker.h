/*
 * nullPointerExceptionChecker.h
 *
 *  Created on: 27 мая 2014 г.
 *      Author: egp
 */

#ifndef NULLPOINTEREXCEPTIONCHECKER_H_
#define NULLPOINTEREXCEPTIONCHECKER_H_

namespace fundament_exceptions {

class nullPointerExceptionChecker {
public:
	static void checkForNullPointer(void* ptr);
};

}  // namespace fundament

#endif /* NULLPOINTEREXCEPTIONCHECKER_H_ */
