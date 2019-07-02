/*
 * graphstore.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef COMP_GRAPHSTORE_H_
#define COMP_GRAPHSTORE_H_

#include "pegraph.h"
//#include "peGraphPointer.h"
//#include "cfg_compute.h"

using namespace std;

class GraphStore{

	friend std::ostream & operator<<(std::ostream & strm, const GraphStore& graphstore) {
		strm << "Graphstore<<<<\n============================================" << endl;
		graphstore.print(strm);
		strm << "============================================" << endl;
		return strm;
	}

public:
    GraphStore(){}

    virtual ~GraphStore(){}

    virtual PEGraph* retrieve(PEGraph_Pointer graph_pointer) = 0;

    virtual void update(PEGraph_Pointer graph_pointer, PEGraph* pegraph) = 0;

    virtual void loadGraphStore(const string& file_singleton) = 0;

    void addOneSingleton(vertexid_t t) {
        this->singletonSet.insert(t);
    }

    inline bool isSingleton(vertexid_t vid) { return singletonSet.find(vid)!= singletonSet.end();}

    std::string toString(){
    	std::ostringstream strm;
		strm << "Graphstore<<<<\n============================================" << endl;
		toString_sub(strm);
		strm << "============================================" << endl;
		return strm.str();
    }


protected:
    std::set<vertexid_t> singletonSet;

    virtual void print(std::ostream& str) const = 0;

    virtual void toString_sub(std::ostringstream& strm) const = 0;

private:
    //	void add();



};



#endif /* COMP_GRAPHSTORE_H_ */
