#ifndef TIMER_SUM_HPP
#define TIMER_SUM_HPP


#include "boost/date_time/posix_time/posix_time.hpp"
#include "../common/CommonLibs.hpp"

// This class represents a timer to calculate the time elapsed
class Timer_sum {
    
    public:
        /* Class Constructors & Destructor */
        Timer_sum()
            : title("Untitle"), wall_ms_string("0"), cpu_ms_string("0") {
        	startTime = boost::posix_time::second_clock::local_time();
        	startClock = std::clock();
        	time_sum = 0;
        	clock_sum = 0;
        }


        Timer_sum(const std::string& title)
            : title(title), wall_ms_string("0"), cpu_ms_string("0") {
        	startTime = boost::posix_time::second_clock::local_time();
        	startClock = std::clock();
        	time_sum = 0;
        	clock_sum = 0;
        }


        ~Timer_sum() {

        }

        void start(){
        	startTime = boost::posix_time::second_clock::local_time();
        	startClock = std::clock();
        }

        void end(){
        	time_sum += (boost::posix_time::second_clock::local_time() - startTime).total_milliseconds();
        	clock_sum += std::clock() - startClock;
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
            auto clocks = clock_sum;
            double millisec_clock = clocks / (CLOCKS_PER_SEC / 1000);
            double sec_clock = clocks / CLOCKS_PER_SEC;
            double minute_clock = sec_clock / 60.0;
            double hour_clock = minute_clock / 60.0;

            auto millisecs = time_sum;
            auto secs = millisecs / 1000.0;
            auto mins = secs / 60.0;
            auto hours = mins / 60.0;

            this->wall_ms_string = to_string_with_precision(millisecs, 3);
            this->cpu_ms_string = to_string_with_precision(millisec_clock, 3);

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


        inline std::string getWallTimeString(){
        	return this->wall_ms_string;
        }

        inline std::string getCPUTimeString(){
        	return this->cpu_ms_string;
        }

    private:
        /* Declaring Variables */
        std::clock_t startClock;
        boost::posix_time::ptime startTime;
        std::string title;

        std::string wall_ms_string;
        std::string cpu_ms_string;

        std::clock_t clock_sum;
        boost::posix_time::time_duration::tick_type time_sum;

};

#endif // TIMER_SUM_HPP
