//
// Created by dell on 8/7/19.
//

#ifndef BACKEND_MYTIMER_H
#define BACKEND_MYTIMER_H


class myTimer {
private:
    static int count_combine_synchronous;
    static int count_transfer;
    static float time_combine_synchronous;
    static float time_transfer;

public:
    myTimer();

    int getCountCombineSynchronous() const;

    void setCountCombineSynchronous(int countCombineSynchronous);

    int getCountTransfer() const;

    void setCountTransfer(int countTransfer);

    float getTimeCombineSynchronous() const;

    void setTimeCombineSynchronous(float timeCombineSynchronous);

    float getTimeTransfer() ;

    void setTimeTransfer(float timeTransfer);

    void addCountCombineSynchronous(float time);

};



#endif //BACKEND_MYTIMER_H
