/*
 * worklist.h
 *
 *  Created on: May 22, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CONCURRENT_WORKSET_H_
#define COMP_CONCURRENT_WORKSET_H_

#include "../common/CommonLibs.hpp"
#include "concurrent_worklist.h"

//template <typename T>
class Concurrent_Workset : public Concurrent_Worklist {


private:
    std::unordered_set<CFGNode*> set;


protected:

    void print(std::ostream& str) const {
//    	str << "size=" << this->size() << "; ";
		for(auto& it: set){
			str << *it << ", ";
		}
    }

public:
    Concurrent_Workset() {}

    ~Concurrent_Workset(){}

    bool isEmpty() {
        return set.empty();
    }

    void push_atomic(CFGNode* item) {
        std::unique_lock < std::mutex > lock(mutex);
        set.insert(item);
    }

    CFGNode* pop_atomic() {
        std::unique_lock < std::mutex > lock(mutex);
        if (set.empty()) {
            return nullptr;
        } else {
        	CFGNode* item = *(set.begin());
        	set.erase(item);
            return item;
        }
    }

    int size() const {
        return set.size();
    }


};



#endif /* COMP_CONCURRENT_WORKLIST_H_ */
