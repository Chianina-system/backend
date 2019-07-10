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
//	CFG(Partition* part);
    virtual ~CFG(){}

    virtual std::vector<CFGNode*> getPredesessors(const CFGNode* node) const = 0;
    virtual std::vector<CFGNode*> getSuccessors(const CFGNode* node) const = 0;
    virtual std::vector<CFGNode*> getNodes() const = 0;
    virtual std::vector<CFGNode*> getEntryNodes() const = 0;
    virtual void loadCFG(const string& file_cfg, const string& file_stmt) = 0;
//    virtual void loadCFG(const string& file_cfg, const string& file_stmt, const string& file_mirrors_in, const string& file_mirrors_out) = 0;


protected:
    virtual void print(std::ostream& str) const = 0;


private:



};



#endif /* COMP_CFG_H_ */
