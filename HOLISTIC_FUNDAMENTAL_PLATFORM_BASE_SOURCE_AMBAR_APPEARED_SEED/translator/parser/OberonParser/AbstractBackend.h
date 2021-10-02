/*
 * AbstractBackend.h
 *
 *  Created on: 03.01.2013
 *      Author: egp
 */

#ifndef ABSTRACTBACKEND_H_
#define ABSTRACTBACKEND_H_

class AbstractBackend {
public:
	enum TYPE {interpreter};
	AbstractBackend();
	virtual TYPE getType()=0;
	virtual ~AbstractBackend();
};

#endif /* ABSTRACTBACKEND_H_ */
