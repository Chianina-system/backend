/*
 * cfg.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_H_
#define COMP_CFG_H_

#include "cfg_node.h"

typedef unsigned Partition;

class CFG{

public:
//    CFG();
//	CFG(Partition* part);
//    virtual ~CFG();


    virtual std::vector<CFGNode*> getPredesessors(const CFGNode* node) const = 0;
    virtual std::vector<CFGNode*> getSuccessors(const CFGNode* node) const = 0;
    virtual std::vector<CFGNode*> getNodes() const = 0;
    virtual void addOneNode(CFGNode* Node) = 0;
    virtual void addOnePred(CFGNode* succ, CFGNode* pred) = 0;
    virtual void addOneSucc(CFGNode* pred, CFGNode* succ) = 0;



private:



};



#endif /* COMP_CFG_H_ */
