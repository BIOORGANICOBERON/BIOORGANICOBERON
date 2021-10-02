/*
 * FileInputStream.h
 *
 *  Created on: 28 мая 2014 г.
 *      Author: egp
 */

#ifndef FILEINPUTSTREAM_H_
#define FILEINPUTSTREAM_H_

#include "InputStream.h"

class FileInputStream: public InputStream {
public:
	FileInputStream(wchar_t* fileName);
	virtual ~FileInputStream();
};

#endif /* FILEINPUTSTREAM_H_ */
