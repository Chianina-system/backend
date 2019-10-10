/*
 * Timer_wrapper_ooc.h
 *
 *  Created on: Oct 6, 2019
 *      Author: dell
 */

#ifndef UTILITY_TIMER_WRAPPER_OOC_H_
#define UTILITY_TIMER_WRAPPER_OOC_H_

#include "timer_sum_sync.hpp"
#include "timer_sum.hpp"

class Timer_wrapper_ooc {

public:

	Timer_wrapper_ooc(){
		load_sum = new Timer_sum("load");
		compute_sum = new Timer_sum("compute");
		pass_sum = new Timer_sum("pass");
		clean_sum = new Timer_sum("clean");

		edge_compute_sum = new Timer_sum("compute-edge-compute");
		edge_update_sum = new Timer_sum("compute-edge-update");

	}

	~Timer_wrapper_ooc(){
    	delete load_sum;
    	delete compute_sum;
    	delete pass_sum;
    	delete clean_sum;

    	delete edge_compute_sum;
    	delete edge_update_sum;
	}


	void print(){
		load_sum->print();
		compute_sum->print();
		pass_sum->print();
		clean_sum->print();

		edge_compute_sum->print();
		edge_update_sum->print();
	}


	Timer_sum* getComputeSum() const {
		return compute_sum;
	}

	Timer_sum* getLoadSum() const {
		return load_sum;
	}

	Timer_sum* getPassSum() const {
		return pass_sum;
	}

	Timer_sum* getEdgeComputeSum() const {
		return edge_compute_sum;
	}

	Timer_sum* getEdgeUpdateSum() const {
		return edge_update_sum;
	}

	Timer_sum* getCleanSum() const {
		return clean_sum;
	}

private:
	Timer_sum* load_sum;
	Timer_sum* compute_sum;
	Timer_sum* pass_sum;
	Timer_sum* clean_sum;

	Timer_sum* edge_compute_sum;
	Timer_sum* edge_update_sum;


};



#endif /* UTILITY_TIMER_WRAPPER_OOC_H_ */
