/*
 * exception.h
 *
 *  Created on: 27 мая 2014 г.
 *      Author: egp
 */

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include "../entity.h"
#include <string>
#include "stdio.h"

namespace fundament_exceptions {

	class exception : public fundament::entity {
		std::wstring* message;
	public:
		exception();
		exception(std::wstring* message_);
		virtual void finalize();
	};
}

#endif /* EXCEPTION_H_ */
