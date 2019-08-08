//
// Created by dell on 8/7/19.
//

#include "myTimer.h"


void myTimer::addCountCombineSynchronous() {
    count_combine_synchronous++;
}

void myTimer::addCountTransfer() {
    count_transfer++;
}

void myTimer::addDurationCombineSynchronous(double duration) {
    duration_combine_synchronous += duration;
}

void myTimer::addDurationTransfer(double duration) {
    duration_transfer += duration;
}




//void myTimer::addCountPegComputeAdd() {
//    count_peg_compute_add++;
//}
//
//void myTimer::addDurationPegComputeAdd(double duration) {
//    duration_peg_compute_add += duration;
//}
//
//void myTimer::addCountPegComputeDeleted() {
//    count_peg_compute_delete++;
//}
//
//void myTimer::addDurationPegComputeDeleted(double duration) {
//    duration_peg_compute_delete += duration;
//
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
