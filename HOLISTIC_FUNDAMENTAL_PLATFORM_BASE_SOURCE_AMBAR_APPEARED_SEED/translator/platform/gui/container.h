/*
 * container.h
 *
 *  Created on: 27 мая 2014 г.
 *      Author: egp
 */

#ifndef CONTAINER_H_
#define CONTAINER_H_

#include "control.h"
#include <vector>

using namespace std;

namespace gui {

	class container: public control {
		vector<control*>* components;
	public:
		container();
		void addComponent(control* component);
		void insertComponentAt(size_t index, control* component);
		void removeComponentAt(size_t index);
		size_t getComponentCount();
		virtual ~container();
	};

}  // namespace gui

#endif /* CONTAINER_H_ */
