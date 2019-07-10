/*
 * buffer.h
 *
 *  Created on: Jul 10, 2019
 *      Author: zqzuo
 */

#ifndef COMP_BUFFER_H_
#define COMP_BUFFER_H_

#include "../common/CommonLibs.hpp"

class Buffer{

private:
	char* buf;
	size_t size;

public:
	Buffer(size_t s){
		this->size = s;
		this->buf = new char[size];
	}

	~Buffer(){
		delete[] buf;
	}

};


#endif /* COMP_BUFFER_H_ */
