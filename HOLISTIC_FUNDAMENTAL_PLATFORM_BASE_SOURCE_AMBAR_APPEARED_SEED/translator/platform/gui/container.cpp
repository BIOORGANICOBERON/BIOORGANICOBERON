/*
 * container.cpp
 *
 *  Created on: 27 мая 2014 г.
 *      Author: egp
 */

#include "container.h"
#include "assert.h"

using namespace gui;
using namespace std;

container::container() {
	components=new vector<control*>();
	assert(components!=0);
}

container::~container() {
	delete components; components=0;
}

