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

	template <typename T>
	static std::string toString_vector(std::vector<T*> vector){
		std::ostringstream strm;
		strm << "[";
		for(auto& it: vector){
			strm << *it << ", ";
		}
		strm << "]\n";
		return strm.str();
	}


};



#endif /* UTILITY_STATICPRINTER_H_ */
