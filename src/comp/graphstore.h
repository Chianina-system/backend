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

class GraphStore{

public:
//    GraphStore(){}
    virtual ~GraphStore(){}

    virtual PEGraph* retrieve(PEGraph_Pointer graph_pointer) = 0;

    virtual void update(PEGraph_Pointer graph_pointer, PEGraph* pegraph) = 0;



private:
    //	void add();


};



#endif /* COMP_GRAPHSTORE_H_ */
