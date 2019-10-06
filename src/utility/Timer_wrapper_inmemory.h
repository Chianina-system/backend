/*
 * Timer_wrapper.h
 *
 *  Created on: Oct 5, 2019
 *      Author: dell
 */

#ifndef UTILITY_TIMER_WRAPPER_INMEMORY_H_
#define UTILITY_TIMER_WRAPPER_INMEMORY_H_

#include "timer_sum_sync.hpp"

class Timer_wrapper_inmemory {

public:

	Timer_wrapper_inmemory(){
		merge_sum = new Timer_sum_sync("merge");
		transfer_sum = new Timer_sum_sync("transfer");
		propagate_sum = new Timer_sum_sync("propagate");

		strongupdate_sum = new Timer_sum_sync("transfer-strong-update");
		add_sum = new Timer_sum_sync("transfer-edge-addition");

		add_direct_sum = new Timer_sum_sync("direct-edges-add");
		add_init_sum = new Timer_sum_sync("init-add");
		add_compute_sum = new Timer_sum_sync("compute-add");
		add_postprocess_sum = new Timer_sum_sync("postprocess-add");

		add_compute_join_sum = new Timer_sum_sync("compute-add-join");
		add_compute_merge_sum = new Timer_sum_sync("compute-add-merge");

		add_compute_join_collect_sum = new Timer_sum_sync("compute-add-join-collect");
		add_compute_join_merge_sum = new Timer_sum_sync("compute-add-join-merge");
		add_compute_join_post_sum = new Timer_sum_sync("compute-add-join-post");
	}

	~Timer_wrapper_inmemory(){
    	delete merge_sum;
    	delete transfer_sum;

    		delete strongupdate_sum;
    		delete add_sum;

				delete add_direct_sum;
				delete add_init_sum;
				delete add_compute_sum;
				delete add_postprocess_sum;

					delete add_compute_join_sum;
					delete add_compute_merge_sum;

						delete add_compute_join_collect_sum;
						delete add_compute_join_merge_sum;
						delete add_compute_join_post_sum;

    	delete propagate_sum;
	}


	void print(){
		merge_sum->print();
		transfer_sum->print();

			strongupdate_sum->print();
			add_sum->print();

				add_direct_sum->print();
				add_init_sum->print();
				add_compute_sum->print();
				add_postprocess_sum->print();

					add_compute_join_sum->print();
					add_compute_merge_sum->print();

						add_compute_join_collect_sum->print();
						add_compute_join_merge_sum->print();
						add_compute_join_post_sum->print();

		propagate_sum->print();
	}

	Timer_sum_sync* getAddComputeJoinSum() const {
		return add_compute_join_sum;
	}

	Timer_sum_sync* getAddComputeMergeSum() const {
		return add_compute_merge_sum;
	}

	Timer_sum_sync* getAddComputeSum() const {
		return add_compute_sum;
	}

	Timer_sum_sync* getAddDirectSum() const {
		return add_direct_sum;
	}

	Timer_sum_sync* getAddInitSum() const {
		return add_init_sum;
	}

	Timer_sum_sync* getAddPostprocessSum() const {
		return add_postprocess_sum;
	}

	Timer_sum_sync* getAddSum() const {
		return add_sum;
	}

	Timer_sum_sync* getMergeSum() const {
		return merge_sum;
	}

	Timer_sum_sync* getPropagateSum() const {
		return propagate_sum;
	}

	Timer_sum_sync* getStrongupdateSum() const {
		return strongupdate_sum;
	}

	Timer_sum_sync* getTransferSum() const {
		return transfer_sum;
	}

	Timer_sum_sync* getAddComputeJoinCollectSum() const {
		return add_compute_join_collect_sum;
	}

	Timer_sum_sync* getAddComputeJoinMergeSum() const {
		return add_compute_join_merge_sum;
	}

	Timer_sum_sync* getAddComputeJoinPostSum() const {
		return add_compute_join_post_sum;
	}

private:
	Timer_sum_sync* merge_sum;
	Timer_sum_sync* transfer_sum;
	Timer_sum_sync* propagate_sum;

	Timer_sum_sync* strongupdate_sum;
	Timer_sum_sync* add_sum;

	Timer_sum_sync* add_direct_sum;
	Timer_sum_sync* add_init_sum;
	Timer_sum_sync* add_compute_sum;
	Timer_sum_sync* add_postprocess_sum;

	Timer_sum_sync* add_compute_join_sum;
	Timer_sum_sync* add_compute_merge_sum;

	Timer_sum_sync* add_compute_join_collect_sum;
	Timer_sum_sync* add_compute_join_merge_sum;
	Timer_sum_sync* add_compute_join_post_sum;

};



#endif /* UTILITY_TIMER_WRAPPER_INMEMORY_H_ */
