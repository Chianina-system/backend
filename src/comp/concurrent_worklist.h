/*
 * worklist.h
 *
 *  Created on: May 22, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CONCURRENT_WORKLIST_H_
#define COMP_CONCURRENT_WORKLIST_H_

#include "../common/CommonLibs.hpp"

template <typename T>
class Concurrent_Worklist{

//	friend std::ostream & operator<<(std::ostream & strm, const Concurrent_Worklist& worklist) {
//		strm << "worklist: [";
//		//TODO:
//		strm << "]";
//		return strm;
//	}

private:
    std::queue<T> queue;
    std::mutex mutex;

//    void print(){
//		cout << "worklist: [";
//		for(auto& it: queue){
//			cout << it << ", ";
//		}
//		cout << "]";
//    }

public:
    Concurrent_Worklist() {}

//		Concurrent_Worklist(const size_t _capacity) {}

    inline bool isEmpty() {
        return queue.empty();
    }

    void push(const T & item) {
        queue.push(item);
    }

    void push_atomic(const T & item) {
        std::unique_lock < std::mutex > lock(mutex);
        queue.push(item);
    }

    bool pop_atomic(T& item) {
        std::unique_lock < std::mutex > lock(mutex);
        if (queue.empty()) {
            return false;
        } else {
            item = queue.front();
            queue.pop();
            return true;
        }
    }

    int size() {
        return queue.size();
    }


};



#endif /* COMP_CONCURRENT_WORKLIST_H_ */
