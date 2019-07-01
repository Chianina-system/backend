/*
 * worklist.h
 *
 *  Created on: May 22, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CONCURRENT_WORKQUEUE_H_
#define COMP_CONCURRENT_WORKQUEUE_H_

#include "../common/CommonLibs.hpp"
#include "concurrent_worklist.h"

//template <typename T>
class Concurrent_Workqueue: public Concurrent_Worklist {

private:
    std::queue<CFGNode*> queue;


protected:
    void print(std::ostream& str) const {
//		for(auto& it: queue){
//			cout << it << ", ";
//		}
    }

public:
    Concurrent_Workqueue() {}

    ~Concurrent_Workqueue(){}

    bool isEmpty() {
        return queue.empty();
    }

    void push_atomic(CFGNode* item) {
        std::unique_lock < std::mutex > lock(mutex);
        queue.push(item);
    }

    CFGNode* pop_atomic() {
        std::unique_lock < std::mutex > lock(mutex);
        if (queue.empty()) {
            return nullptr;
        } else {
        	CFGNode* item = queue.front();
            queue.pop();
            return item;
        }
    }

    int size() const {
        return queue.size();
    }


};



#endif /* COMP_CONCURRENT_WORKLIST_H_ */
