//
// Created by dell on 8/7/19.
//

#include <mutex>
#include "myTimer.h"

myTimer::myTimer(int countCombineSynchronous, int countTransfer, float timeCombineSynchronous, float timeTransfer)
        : count_combine_synchronous(countCombineSynchronous), count_transfer(countTransfer),
          time_combine_synchronous(timeCombineSynchronous), time_transfer(timeTransfer) {}

int myTimer::getCountCombineSynchronous()  {
    return count_combine_synchronous;
}

void myTimer::setCountCombineSynchronous(int countCombineSynchronous) {
    count_combine_synchronous = countCombineSynchronous;
}

int myTimer::getCountTransfer() {
    return count_transfer;
}

void myTimer::setCountTransfer(int countTransfer) {
    count_transfer = countTransfer;
}

float myTimer::getTimeCombineSynchronous() {
    return time_combine_synchronous;
}

void myTimer::setTimeCombineSynchronous(float timeCombineSynchronous) {
    time_combine_synchronous = timeCombineSynchronous;
}


void myTimer::setTimeTransfer(float timeTransfer) {
    time_transfer = timeTransfer;
}



myTimer* myTimer::getInstance() {
    myTimer* tmp = m_instance.load(std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acquire);//获取内存fence
    if (tmp == nullptr) {
        std::lock_guard<std::mutex> lock(m_mutex);
        tmp = m_instance.load(std::memory_order_relaxed);
        if (tmp == nullptr) {
            tmp = new myTimer;
            std::atomic_thread_fence(std::memory_order_release);//释放内存fence
            m_instance.store(tmp, std::memory_order_relaxed);
        }
    }
    return tmp;
}
