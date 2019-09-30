#ifndef TIMER_DIFF_HPP
#define TIMER_DIFF_HPP


#include "boost/date_time/posix_time/posix_time.hpp"
#include "../common/CommonLibs.hpp"

// This class represents a timer to calculate the time elapsed
class Timer_diff {
    
    public:
        /* Class Constructors & Destructor */
        Timer_diff(){
        	startTime = boost::posix_time::second_clock::local_time();
        	startClock = std::clock();
        	time_diff = 0;
        	clock_diff = 0;
        }


        ~Timer_diff() {

        }

        void start(){
        	startTime = boost::posix_time::second_clock::local_time();
        	startClock = std::clock();
        }

        void end(){
        	time_diff = (boost::posix_time::second_clock::local_time() - startTime).total_milliseconds();
        	clock_diff = std::clock() - startClock;
        }

        inline std::clock_t getClockDiff(){
        	return clock_diff;
        }

        inline boost::posix_time::time_duration::tick_type getTimeDiff(){
        	return time_diff;
        }


    private:
        /* Declaring Variables */
        std::clock_t startClock;
        boost::posix_time::ptime startTime;

        std::clock_t clock_diff;
        boost::posix_time::time_duration::tick_type time_diff;
};

#endif // TIMER_DIFF_HPP
