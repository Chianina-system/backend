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
	CFG();
//	CFG(Partition* part);
	~CFG(){}


	std::vector<CFGNode*> getPredesessors(const CFGNode* node) const;
	std::vector<CFGNode*> getSuccessors(const CFGNode* node) const;

	std::vector<CFGNode*> getNodes();


private:


};



#endif /* COMP_CFG_H_ */
