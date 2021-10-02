/*
 * frame.cpp
 *
 *  Created on: 27 мая 2014 г.
 *      Author: egp
 */

#include "frame.h"
#include "assert.h"

using namespace gui;

frame::frame() {
	rootPane=new container();
	assert(rootPane!=0);
}

void frame::finalize() {delete rootPane; rootPane=0; window::finalize();}

container* frame::getRootPane(){return rootPane;}
