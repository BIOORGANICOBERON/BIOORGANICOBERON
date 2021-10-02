/*
 * BufferedInputStream.h
 *
 *  Created on: 28 мая 2014 г.
 *      Author: egp
 */

#ifndef BUFFEREDINPUTSTREAM_H_
#define BUFFEREDINPUTSTREAM_H_

class BufferedInputStream : public InputStream {
public:
	BufferedInputStream(InputStream *is, size_t bufSize);
	virtual ~BufferedInputStream();
};

#endif /* BUFFEREDINPUTSTREAM_H_ */
