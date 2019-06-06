/*
 * art.h
 *
 *  Created on: May 22, 2019
 *      Author: zqzuo
 */

#ifndef COMP_ART_H_
#define COMP_ART_H_

#include "graphstore.h"

class ART: public GraphStore {

public:
	~ART(){}

	PEGraph* retrieve(PEGraph_Pointer graph_pointer);

    void update(PEGraph_Pointer graph_pointer, PEGraph* pegraph);



private:



};




#endif /* COMP_ART_H_ */
