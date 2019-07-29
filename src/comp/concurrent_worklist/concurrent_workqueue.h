/*
 * worklist.h
 *
 *  Created on: May 22, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CONCURRENT_WORKLIST_CONCURRENT_WORKQUEUE_H_
#define COMP_CONCURRENT_WORKLIST_CONCURRENT_WORKQUEUE_H_

#include "concurrent_worklist.h"

template <typename T>
class Concurrent_Workqueue: public Concurrent_Worklist<T> {

private:
    std::queue<T> queue;


protected:
//    void print(std::ostream& str) const {
//
//    }

    void toString_sub(std::ostringstream& strm) const {

    }


public:
    Concurrent_Workqueue() {}

    ~Concurrent_Workqueue(){}

    bool isEmpty() {
        return queue.empty();
    }

    void push(T item) {
        queue.push(item);
    }

    bool pop(T & item) {
        if (queue.empty()) {
            return false;
        } else {
        	item = queue.front();
            queue.pop();
            return true;
        }
    }

    int size() const {
        return queue.size();
    }


};



#endif /* COMP_CONCURRENT_WORKLIST_H_ */
