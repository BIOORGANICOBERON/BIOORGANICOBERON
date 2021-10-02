/*
 * window.h
 *
 *  Created on: 27 мая 2014 г.
 *      Author: egp
 */

#ifndef WINDOW_H_
#define WINDOW_H_

#include "control.h"

using namespace fundament;

namespace gui {

	class window : public control {
	public:
		window();
		virtual void finalize();
	};

}  // namespace gui

#endif /* WINDOW_H_ */
