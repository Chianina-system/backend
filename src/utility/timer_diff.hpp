#ifndef TIMER_DIFF_HPP
#define TIMER_DIFF_HPP


#include "boost/date_time/posix_time/posix_time.hpp"
#include "../common/CommonLibs.hpp"

// This class represents a timer to calculate the time elapsed
class Timer_diff {
    
    public:
        /* Class Constructors & Destructor */
        Timer_diff(): title(" "){
        	startTime = boost::posix_time::second_clock::local_time();
        	startClock = std::clock();
        	time_diff = 0;
        	clock_diff = 0;
        }

        Timer_diff(const std::string& title): title(title){
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

        void print(){
        	//print out time cost information
        	std::cout << "\n\n";
        	std::cout << "------------------------------ {" << title << "} time usage ------------------------------" << std::endl;
        	std::cout << result() << std::endl;
        	std::cout << "------------------------------ {" << title << "} time usage ------------------------------" << std::endl;
        	std::cout << "\n\n";
        }


        /* Public Methods */
        std::string result() {
            auto clocks = clock_diff;
            double millisec_clock = clocks / (CLOCKS_PER_SEC / 1000);
            double sec_clock = clocks / CLOCKS_PER_SEC;
            double minute_clock = sec_clock / 60.0;
            double hour_clock = minute_clock / 60.0;

            auto millisecs = time_diff;
            auto secs = millisecs / 1000.0;
            auto mins = secs / 60.0;
            auto hours = mins / 60.0;

            return
            	"Wall time for \"" + title + "\": " +
        		to_string_with_precision(millisecs, 3) + " ms; " +
        		to_string_with_precision(secs, 3) + " s; " +
            	to_string_with_precision(mins, 3) + " m; " +
            	to_string_with_precision(hours, 3) + " h\n" +
            	"CPU time for \"" + title + "\": " +
                to_string_with_precision(millisec_clock, 3) + " ms; " +
                to_string_with_precision(sec_clock, 3) + " s; " +
                to_string_with_precision(minute_clock, 3) + " m; " +
                to_string_with_precision(hour_clock, 3) + " h";
        }

        template <typename T>
        std::string to_string_with_precision(const T a_value, const int& n) {
            std::ostringstream out;
            out << std::fixed;
            out << std::setprecision(n) << a_value;
            return out.str();
        }

        inline std::clock_t getClockDiff(){
        	return clock_diff;
        }

        inline boost::posix_time::time_duration::tick_type getTimeDiff(){
        	return time_diff;
        }


    private:
        /* Declaring Variables */
        std::string title;

        std::clock_t startClock;
        boost::posix_time::ptime startTime;

        std::clock_t clock_diff;
        boost::posix_time::time_duration::tick_type time_diff;
};

#endif // TIMER_DIFF_HPP
