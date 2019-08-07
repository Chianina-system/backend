//
// Created by dell on 8/7/19.
//

#ifndef BACKEND_MYTIMER_H
#define BACKEND_MYTIMER_H


#include <atomic>

class myTimer {
private:
    int count_combine_synchronous;
    int count_transfer;
    float time_combine_synchronous;
    float time_transfer;
    myTimer(int countCombineSynchronous = 0, int countTransfer= 0, float timeCombineSynchronous= 0, float timeTransfer= 0);
    myTimer(const myTimer& other);



public:
    static myTimer* getInstance();
    //C++ 11版本之后的跨平台实现 (volatile)
    static std::atomic<myTimer*> m_instance;
    static std::mutex m_mutex;

    int getCountCombineSynchronous() ;

    void setCountCombineSynchronous(int countCombineSynchronous);

    int getCountTransfer() ;

    void setCountTransfer(int countTransfer);

    float getTimeCombineSynchronous() ;

    void setTimeCombineSynchronous(float timeCombineSynchronous);

    float getTimeTransfer() ;

    void setTimeTransfer(float timeTransfer);

};





#endif //BACKEND_MYTIMER_H
