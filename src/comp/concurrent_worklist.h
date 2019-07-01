/*
 * worklist.h
 *
 *  Created on: May 22, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CONCURRENT_WORKLIST_H_
#define COMP_CONCURRENT_WORKLIST_H_

#include "../common/CommonLibs.hpp"
#include "cfg_node.h"

//template <typename T>
class Concurrent_Worklist{

	friend std::ostream & operator<<(std::ostream & strm, const Concurrent_Worklist& worklist) {
		strm << "worklist: [";
		strm << "size=" << worklist.size() << "; ";
		//TODO:
		worklist.print(strm);
		strm << "]";
		return strm;
	}


public:
    Concurrent_Worklist() {}

    virtual ~Concurrent_Worklist(){}

    virtual bool isEmpty() = 0;

    virtual void push_atomic(CFGNode* item) = 0;

    virtual CFGNode* pop_atomic() = 0;

    virtual int size() const = 0;


protected:
    std::mutex mutex;


    virtual void print(std::ostream& str) const = 0;

};



#endif /* COMP_CONCURRENT_WORKLIST_H_ */
