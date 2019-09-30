/*
 * MyArray.h
 *
 *  Created on: Sep 29, 2019
 *      Author: dell
 */

#ifndef COMP_GRAPHSTORE_MYARRAY_H_
#define COMP_GRAPHSTORE_MYARRAY_H_

class MyArray {

public:
	MyArray(unsigned int l){
		len = l;
		data = new int[len];
		index = 0;
	}


	~MyArray(){

	}

	void clear(){
		if(data){
			delete[] data;
			data = nullptr;
			len = 0;
			index = 0;
		}
	}

	void insert(int d){
		data[index] = d;
		index++;
	}

	int getData(unsigned index) const {
		return data[index];
	}

	int* getData(){
		return data;
	}

	void shallowCopy(MyArray* another){
		clear();
		data = another->getData();
		len = another->getLength();
		index = another->getIndex();
	}

	inline unsigned getLength() const {
		return len;
	}

	inline unsigned getIndex() const {
		return index;
	}


private:
	int* data;
	unsigned int len;
	unsigned int index;



};



#endif /* COMP_GRAPHSTORE_MYARRAY_H_ */
