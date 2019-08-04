/*
 * worklist.h
 *
 *  Created on: May 22, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CONCURRENT_WORKLIST_CONCURRENT_WORKSET_H_
#define COMP_CONCURRENT_WORKLIST_CONCURRENT_WORKSET_H_

#include "concurrent_worklist.h"

template <typename T>
struct cfgnode_pointer_compare {
    bool operator() (const T& lhs, const T& rhs) const {
        return lhs < rhs;
    }
};


template <typename T>
class Concurrent_Workset : public Concurrent_Worklist<T> {


private:
	std::set<T, cfgnode_pointer_compare<T>> set;


protected:

//    void print(std::ostream& str) const {
////    	str << "size=" << this->size() << "; ";
////		for(auto& it: set){
////			str << *it << ", ";
////		}
//    }

    void toString_sub(std::ostringstream& strm) const {
//		for(auto& it: set){
//			if(CFGNode* node = dynamic_cast<CFGNode*>(it)){
//				strm << *node << ", ";
//			}
//		}
    }



public:
    Concurrent_Workset() {}

    ~Concurrent_Workset(){}

    bool isEmpty() {
        return set.empty();
    }

    void push(T item) {
        set.insert(item);
    }

    bool pop(T& item) {
        if (set.empty()) {
            return false;
        } else {
        	item = *(set.begin());
        	set.erase(item);
            return true;
        }
    }

    int size() const {
        return set.size();
    }

    std::set<T>* getSet(){
    	return new std::set<T>(set.begin(), set.end());
    }


};



#endif /* COMP_CONCURRENT_WORKLIST_H_ */
