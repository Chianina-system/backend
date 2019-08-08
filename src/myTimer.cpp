//
// Created by dell on 8/7/19.
//

#include "myTimer.h"

myTimer::myTimer() {}


void myTimer::addCountCombineSynchronous() {
    count_combine_synchronous += 1;
}

void myTimer::addCountTransfer() {
    count_transfer += 1;
}

void myTimer::addDurationCombineSynchronous(double duration) {
    duration_combine_synchronous += duration;
}

void myTimer::addDurationTransfer(double duration) {
    duration_transfer += duration;
}

int myTimer::getCountCombineSynchronous() {
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

float myTimer::getDurationCombineSynchronous() {
    return duration_combine_synchronous;
}

void myTimer::setDurationCombineSynchronous(float durationCombineSynchronous) {
    duration_combine_synchronous = durationCombineSynchronous;
}

float myTimer::getDurationTransfer() {
    return duration_transfer;
}

void myTimer::setDurationTransfer(float durationTransfer) {
    duration_transfer = durationTransfer;
}

//int myTimer::getCountPegComputeAdd() {
//    return count_peg_compute_add;
//}
//
//void myTimer::setCountPegComputeAdd(int countPegComputeAdd) {
//    count_peg_compute_add = countPegComputeAdd;
//}
//
//float myTimer::getDurationPegComputeAdd() {
//    return duration_peg_compute_add;
//}
//
//void myTimer::setDurationPegComputeAdd(float durationPegComputeAdd) {
//    duration_peg_compute_add = durationPegComputeAdd;
//}
//
//void myTimer::addCountPegComputeAdd() {
//    count_peg_compute_add++;
//}
//
//void myTimer::addDurationPegComputeAdd(double duration) {
//    duration_peg_compute_add += duration;
//
//}


//void myTimer::addDurationCombineSynchronous(double duration) {
//    duration_combine_synchronous += duration;
//}
//
//void myTimer::addDurationTransfer(double duration) {
//    duration_transfer += duration;
//}

