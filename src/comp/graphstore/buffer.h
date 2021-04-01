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
const long IO_SIZE = 16 * 1024 * 1024; // 16M
const long CHUNK_SIZE = IO_SIZE * 2;
const long PAGE_SIZE = 4 * 1024; // 4K
const int MAX_QUEUE_SIZE = 65536;
const long BUF_SIZE = 9 * 1024 * 1024;

static long get_real_io_size(long io_size, int size_of_unit){
	long real_io_size = io_size - io_size % size_of_unit;
	assert(real_io_size % size_of_unit == 0);
	return real_io_size;
}

class Buffer{

public:
	Buffer(){
		data = (char*) malloc(BUF_SIZE);
		size = 0;
//		capacity = IO_SIZE;
	}

//	Buffer(long real_io_size){
//		capacity = real_io_size;
//		data = (char*) malloc(real_io_size);
//		size = 0;
//	}


	~Buffer(){
		if(data){
			delete[] data;
		}
	}


	char* getData() const {
		return data;
	}

	unsigned int getSize() const {
		return size;
	}

	void setSize(unsigned int s){
		this->size = s;
	}

	void add_size_by(unsigned int added){
		this->size += added;
	}

	bool isFull() const {
//		return size == capacity;
		return size == BUF_SIZE;
	}

	bool isEmpty() const {
		return size == 0;
	}

	unsigned int getCapacity(){
		return BUF_SIZE;
	}

private:
	char* data = NULL;
	unsigned int size = 0;
//	unsigned int capacity = 0;



};


#endif /* COMP_GRAPHSTORE_BUFFER_H_ */
