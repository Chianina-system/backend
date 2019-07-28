/*
 * cfg.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_H_
#define COMP_CFG_H_

#include "cfg_node.h"



class CFG{

	friend std::ostream & operator<<(std::ostream & strm, const CFG& cfg) {
		strm << "\nCFG<<<<\n---------------------" << endl;
		cfg.print(strm);
		strm << "---------------------" << endl;
		return strm;
	}

public:
//    CFG();
    virtual ~CFG(){}

    virtual std::vector<CFGNode*> getPredesessors(const CFGNode* node) const = 0;

    virtual std::vector<CFGNode*> getSuccessors(const CFGNode* node) const = 0;

    inline std::vector<CFGNode*> getNodes() const {
    	return nodes;
    }

    inline vertexid_t getNumberEdges() const {
    	return number_edges;
    }

protected:
    virtual void print(std::ostream& str) const = 0;


    std::vector<CFGNode*> nodes;

    vertexid_t number_edges = 0;

    std::unordered_map<const CFGNode*, std::vector<CFGNode*>> predes;

    std::unordered_map<const CFGNode*, std::vector<CFGNode*>> succes;


};



#endif /* COMP_CFG_H_ */
