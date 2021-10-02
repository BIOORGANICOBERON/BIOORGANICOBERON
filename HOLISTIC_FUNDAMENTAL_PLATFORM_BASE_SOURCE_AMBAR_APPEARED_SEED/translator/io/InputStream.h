/*
 * InputStream.h
 *
 *  Created on: 28 мая 2014 г.
 *      Author: egp
 */

#ifndef INPUTSTREAM_H_
#define INPUTSTREAM_H_

class InputStream {
public:
	InputStream();
	virtual ~InputStream();
	virtual void close();
};

#endif /* INPUTSTREAM_H_ */
