/*
 * frame.h
 *
 *  Created on: 27 мая 2014 г.
 *      Author: egp
 */

#ifndef FRAME_H_
#define FRAME_H_

#include "window.h"
#include "container.h"

namespace gui {

	class frame: public window {
		container* rootPane;
	public:
		frame();
		container* getRootPane();
		virtual void finalize();
	};

}  // namespace gui

#endif /* FRAME_H_ */
