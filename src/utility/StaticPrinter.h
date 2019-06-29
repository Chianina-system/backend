/*
 * StaticPrinter.h
 *
 *  Created on: Jun 29, 2019
 *      Author: zqzuo
 */

#ifndef UTILITY_STATICPRINTER_H_
#define UTILITY_STATICPRINTER_H_

#include "../common/CommonLibs.hpp"

class StaticPrinter{

public:
	template <typename T>
	static void print_vector(std::vector<T*> vector){
		cout << "[";
		for(auto& it: vector){
			cout << *it << ", ";
		}
		cout << "]\n";
	}


};



#endif /* UTILITY_STATICPRINTER_H_ */
