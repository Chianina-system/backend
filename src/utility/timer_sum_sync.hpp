#ifndef TIMER_SUM_SYNC_HPP
#define TIMER_SUM_SYNC_HPP


#include "boost/date_time/posix_time/posix_time.hpp"
#include "../common/CommonLibs.hpp"

// This class represents a timer to calculate the time elapsed
class Timer_sum_sync {
    
    public:
        /* Class Constructors & Destructor */
        Timer_sum_sync()
            : title("Untitle"), wall_ms_string("0"), cpu_ms_string("0") {
        	time_sum = 0;
        	clock_sum = 0;
        }


        Timer_sum_sync(const std::string& title)
            : title(title), wall_ms_string("0"), cpu_ms_string("0") {
        	time_sum = 0;
        	clock_sum = 0;
        }


        ~Timer_sum_sync() {

        }


        void add_locked(std::clock_t clock_diff, boost::posix_time::time_duration::tick_type time_diff){
        	std::lock_guard<std::mutex> lockGuard(mutex);
        	time_sum += time_diff;
        	clock_sum += clock_diff;
        }

        void add(std::clock_t clock_diff, boost::posix_time::time_duration::tick_type time_diff){
        	time_sum += time_diff;
        	clock_sum += clock_diff;
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
        std::string title;

        std::string wall_ms_string;
        std::string cpu_ms_string;

        std::clock_t clock_sum;
        boost::posix_time::time_duration::tick_type time_sum;

        std::mutex mutex;
};

#endif // TIMER_SUM_SYNC_HPP
