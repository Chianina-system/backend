/*
 * cfg.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_H_
#define COMP_CFG_H_

#include "cfg_node.h"
#include "../utility/Logger.hpp"
#include "graphstore/buffer.h"
#include "io_manager.hpp"


class CFG{

	friend std::ostream & operator<<(std::ostream & strm, const CFG& cfg) {
		strm << "\nCFG<<<<\n---------------------" << endl;
		cfg.print(strm);
		strm << "---------------------" << endl;
		return strm;
	}

public:
	CFG(){}

    virtual ~CFG(){
        for (auto &node : nodes) {
            delete node;
        }
    }

//    virtual std::vector<CFGNode*>* getPredesessors(const CFGNode* node) = 0;
//
//    virtual std::vector<CFGNode*>* getSuccessors(const CFGNode* node) = 0;

    std::vector<CFGNode*>* getPredesessors(const CFGNode* node) {
        auto it = predes.find(node);
        if(it != predes.end()){
            return &(it->second);
        }
        else{
//        	return std::vector<CFGNode*>();
        	return nullptr;
//            perror("invalid key!");
//            exit (EXIT_FAILURE);
        }
    }

    std::vector<CFGNode*>* getSuccessors(const CFGNode* node) {
        auto it = succes.find(node);
        if(it != succes.end()){
            return &(it->second);
        }
        else{
//        	return std::vector<CFGNode*>();
        	return nullptr;
//            perror("invalid key!");
//            exit (EXIT_FAILURE);
        }
    }

    inline std::vector<CFGNode*>& getNodes() {
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
