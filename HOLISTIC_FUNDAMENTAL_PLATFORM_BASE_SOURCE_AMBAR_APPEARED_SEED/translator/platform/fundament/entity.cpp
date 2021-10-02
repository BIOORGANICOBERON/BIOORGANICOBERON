/*
 * entity.cpp
 *
 *  Created on: 27 мая 2014 г.
 *      Author: egp
 */

#include "entity.h"

using namespace fundament;

entity::entity() {
}

void entity::finalize(){
}

entity::~entity() {
	finalize();
}

