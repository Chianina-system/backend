//
// Created by dell on 8/7/19.
//

#include "myTimer.h"

myTimer::myTimer(){}

int myTimer::getCountCombineSynchronous() const {
    return count_combine_synchronous;
}

void myTimer::setCountCombineSynchronous(int countCombineSynchronous) {
    count_combine_synchronous = countCombineSynchronous;
}

int myTimer::getCountTransfer() const {
    return count_transfer;
}

void myTimer::setCountTransfer(int countTransfer) {
    count_transfer = countTransfer;
}

float myTimer::getTimeCombineSynchronous() const {
    return time_combine_synchronous;
}

void myTimer::setTimeCombineSynchronous(float timeCombineSynchronous) {
    time_combine_synchronous = timeCombineSynchronous;
}


void myTimer::setTimeTransfer(float timeTransfer) {
    time_transfer = timeTransfer;
}

void myTimer::addCountCombineSynchronous(float time) {
        count_combine_synchronous+=time;
}
