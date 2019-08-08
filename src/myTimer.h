//
// Created by dell on 8/7/19.
//

#ifndef BACKEND_MYTIMER_H
#define BACKEND_MYTIMER_H


class myTimer {
private:


//    peg_compute_add
//    startCompute_add
//
//    peg_compute_delete
//    startCompute_delete

public:

    static int count_combine_synchronous;
    static float duration_combine_synchronous;

    static int count_transfer;
    static float duration_transfer;



//    static int count_transfer;
//    static float duration_transfer;


//    static int count_peg_compute_add;
//    static float duration_peg_compute_add;
//
//    static int count_peg_compute_delete;
//    static float duration_peg_compute_delete;


    static void addCountCombineSynchronous();

    static void addDurationCombineSynchronous(double duration);

    static void addCountTransfer();

    static void addDurationTransfer(double duration);

    static void addCountPegComputeAdd();

    static void addDurationPegComputeAdd(double duration);

    static void addCountPegComputeDeleted();

    static void addDurationPegComputeDeleted(double duration);


};


#endif //BACKEND_MYTIMER_H
