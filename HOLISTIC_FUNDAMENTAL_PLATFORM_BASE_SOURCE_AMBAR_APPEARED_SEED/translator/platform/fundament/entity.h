/*
 * entity.h
 *
 *  Created on: 27 мая 2014 г.
 *      Author: egp
 */

#ifndef ENTITY_H_
#define ENTITY_H_

namespace fundament {

	class entity {
	public:
		entity();
		virtual void finalize();
		virtual ~entity();
	};

}  // namespace fundament

#endif /* ENTITY_H_ */
