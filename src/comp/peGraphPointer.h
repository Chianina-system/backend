/*
 * peGraphPointer.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef COMP_PEGRAPHPOINTER_H_
#define COMP_PEGRAPHPOINTER_H_


class PEGraph_Pointer{


public:

    PEGraph *getGraph() const {
        return graph;
    }

//    bool operator() (const PEGraph_Pointer &rhs) const {
//        return graph == rhs.graph;
//    }

private:
    PEGraph* graph;

};

struct hash_name{
    size_t operator() (const PEGraph_Pointer& p)const{
        using std::hash;
        return hash<int>()(random());
    }
};




#endif /* COMP_PEGRAPHPOINTER_H_ */
