/*
 * worklist.h
 *
 *  Created on: May 22, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CONCURRENT_WORKLIST_CONCURRENT_WORKLIST_H_
#define COMP_CONCURRENT_WORKLIST_CONCURRENT_WORKLIST_H_

#include "../../common/CommonLibs.hpp"
#include "../cfg_node.h"

template <typename T>
class Concurrent_Worklist{

//	friend std::ostream & operator<<(std::ostream & strm, const Concurrent_Worklist<T>& worklist) {
//		strm << "worklist: [";
//		strm << "size=" << worklist.size() << "; ";
//		worklist.print(strm);
//		strm << "]";
//		return strm;
//	}


public:
    Concurrent_Worklist() {}

    virtual ~Concurrent_Worklist(){}

    virtual bool isEmpty() = 0;

    void push_atomic(T item) {
    	std::unique_lock < std::mutex > lock(mutex);
    	push(item);
    }

    bool pop_atomic(T & item){
    	std::unique_lock < std::mutex > lock(mutex);
    	return pop(item);
    }

    virtual int size() const = 0;

    std::string toString(){
    	std::ostringstream strm;
		strm << "worklist: [";
		strm << "size=" << size() << "; ";
		toString_sub(strm);
		strm << "]";
    	return strm.str();
    }

    virtual void push(T item) = 0;
    virtual bool pop(T & item) = 0;

protected:
    std::mutex mutex;


//    virtual void print(std::ostream& str) const = 0;
    virtual void toString_sub(std::ostringstream& strm) const = 0;

};



#endif /* COMP_CONCURRENT_WORKLIST_CONCURRENT_WORKLIST_H_ */
