/*
 * partition_priority.h
 *
 *  Created on: Jul 27, 2019
 *      Author: dell
 */

#ifndef COMP_PARTITION_PRIORITY_PARTITION_PRIORITY_H_
#define COMP_PARTITION_PRIORITY_PARTITION_PRIORITY_H_

#include "../../common/CommonLibs.hpp"

class Priority_partition {

public:
	virtual ~Priority_partition(){
//		cout << "priority-partition destructor" << endl;
	}

	virtual bool schedule(Partition& part) = 0;

	virtual void update_priority(Partition part, int size) = 0;

	virtual void printOutPriorityInfo() = 0;

};


#endif /* COMP_PARTITION_PRIORITY_PARTITION_PRIORITY_H_ */
