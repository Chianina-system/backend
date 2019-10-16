/*
 * buffer.h
 *
 *  Created on: Oct 14, 2019
 *      Author: zzq
 */

#ifndef COMP_GRAPHSTORE_BUFFER_H_
#define COMP_GRAPHSTORE_BUFFER_H_

//#include ""


const size_t BUFFER_CAPACITY = 1000000;
const long IO_SIZE = 16 * 1024 * 1024; // 24M
const long CHUNK_SIZE = IO_SIZE * 2;
const long PAGE_SIZE = 4 * 1024; // 4K
const int MAX_QUEUE_SIZE = 65536;

static long get_real_io_size(long io_size, int size_of_unit){
	long real_io_size = io_size - io_size % size_of_unit;
	assert(real_io_size % size_of_unit == 0);
	return real_io_size;
}


#endif /* COMP_GRAPHSTORE_BUFFER_H_ */
