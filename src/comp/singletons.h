/*
 * singleton.h
 *
 *  Created on: Jul 5, 2019
 *      Author: zqzuo
 */

#ifndef SINGLETON_H_
#define SINGLETON_H_

#include "../common/CommonLibs.hpp"
using namespace std;

class Singletons {

	friend std::ostream & operator<<(std::ostream & strm, Singletons& singleton) {
		strm << "The singleton set: [";
		for (auto & id : singleton.singletonSet) {
			strm << id << ", ";
		}
		strm << "]\n";
		return strm;
	}

public:

    inline void addOneSingleton(vertexid_t t) {
        this->singletonSet.insert(t);
    }

    inline bool isSingleton(vertexid_t vid) {
    	return singletonSet.find(vid)!= singletonSet.end();
    }

    void loadSingletonSet(const string& file_singleton) {
	    //handle the singleton.txt
	    std::ifstream fin;
	    fin.open(file_singleton);
	    if(!fin) {
	        cout << "can't load file_singleton: " << file_singleton << endl;
	        exit (EXIT_FAILURE);
	    }

	    std::string line;
	    while (getline(fin, line) && line != "") {
	        std::stringstream stream(line);
	        std::string id;
	        stream >> id;
	        this->addOneSingleton(atoi(id.c_str()));
	    }
	    fin.close();

    }



private:
    std::set<vertexid_t> singletonSet;


};



#endif /* SINGLETON_H_ */
