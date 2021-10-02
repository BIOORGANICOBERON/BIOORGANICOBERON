/*
 * PreprocessingInputStream.h
 *
 *  Created on: 28 мая 2014 г.
 *      Author: egp
 */

#ifndef PREPROCESSINGINPUTSTREAM_H_
#define PREPROCESSINGINPUTSTREAM_H_

class PreprocessingInputStream : public InputStream {
public:
	PreprocessingInputStream(BufferedInputStream *unpreprocessedIS);
	virtual ~PreprocessingInputStream();
};

#endif /* PREPROCESSINGINPUTSTREAM_H_ */
