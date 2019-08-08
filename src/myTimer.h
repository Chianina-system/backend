//
// Created by dell on 8/7/19.
//

#ifndef BACKEND_MYTIMER_H
#define BACKEND_MYTIMER_H


class myTimer {
private:
    static int count_combine_synchronous;
    static float duration_combine_synchronous;


    static int count_transfer;
    static float duration_transfer;

    static int count_peg_compute_add;
    static float duration_peg_compute_add;

    peg_compute_add
    startCompute_add

    peg_compute_delete
    startCompute_delete

public:
    myTimer();

    static int getCountCombineSynchronous();

    static void setCountCombineSynchronous(int countCombineSynchronous);

    static float getDurationCombineSynchronous();

    static void setDurationCombineSynchronous(float durationCombineSynchronous);

    void addCountCombineSynchronous();

    void addDurationCombineSynchronous(double duration);


    static int getCountTransfer();

    static void setCountTransfer(int countTransfer);

    static float getDurationTransfer();

    static void setDurationTransfer(float durationTransfer);

    void addCountTransfer();

    void addDurationTransfer(double duration);


    static int getCountPegComputeAdd();

    static void setCountPegComputeAdd(int countPegComputeAdd);

    static float getDurationPegComputeAdd();

    static void setDurationPegComputeAdd(float durationPegComputeAdd);

    void addCountPegComputeAdd();

    void addDurationPegComputeAdd(double duration);


};



#endif //BACKEND_MYTIMER_H
