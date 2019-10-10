/*
 * priority_array.h
 *
 *  Created on: Jul 27, 2019
 *      Author: dell
 */

#ifndef COMP_PARTITION_PRIORITY_PRIORITY_ARRAY_H_
#define COMP_PARTITION_PRIORITY_PRIORITY_ARRAY_H_

#include "partition_priority.h"

class Priority_array: public Priority_partition {

public:
	Priority_array(int n_partitions){
		this->num_partitions = n_partitions;
		this->scores = new int[this->num_partitions];
		for(int i = 0; i < this->num_partitions; ++i){
			scores[i] = 0;
		}
	}

	~Priority_array(){
//		cout << "priority-array destructor" << endl;
		if(scores)
			delete[] scores;
//		cout << "priority-array destructor end" << endl;
	}

	bool schedule(Partition& part){
		assert(part <= num_partitions);
		int max = 0;
		for(int i = 0; i < this->num_partitions; i++){
			if(max < scores[i]){
				max = scores[i];
				part = i;
			}
		}
		if(max != 0){
			scores[part] = 0;
			return true;
		}
		else{
			return false;
		}
	}

	void update_priority(Partition part, int size){
		assert(part <= num_partitions);
		scores[part] += size;
	}

	void printOutPriorityInfo(){
		cout << "\nPriority array: {";
		for (int i = 0; i < this->num_partitions; i++) {
			if(scores[i] != 0){
			}
				cout << "<pid=" << i << ", score=" << scores[i] << ">\t";
		}
		cout << "}\n" << endl;
	}


private:
	int num_partitions;
	int* scores;

};



#endif /* COMP_PARTITION_PRIORITY_PRIORITY_ARRAY_H_ */
