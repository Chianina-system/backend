/*
 * worklist.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_COMPUTE_H_
#define COMP_CFG_COMPUTE_H_

#include "graphstore.h"
#include "cfg_map.h"
#include "concurrent_worklist.h"
#include "grammar.h"
#include "peg_compute.h"
#include "computationset.h"
using namespace std;

class CFGCompute{

public:
    unsigned int num_threads;

    bool load(Partition* part, CFG* cfg, GraphStore* graphstore);

    bool load(string file_cfg, string file_peg, string file_singleton, CFG* cfg, GraphStore* graphstore);

    void do_worklist(CFG* cfg, GraphStore* graphstore); //worklist algorithm in parallel



private:
//	CFG* cfg;
//	GraphStore* graphstore;


    void compute(CFG* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1, Concurrent_Worklist<CFGNode*>* worklist_2);


//	void peg_compute(CFG* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist, CFGNode* cfg_node);

    PEGraph* combine(GraphStore* graphstore, std::vector<CFGNode*>& preds);

    PEGraph* transfer(PEGraph* in, Stmt* stmt,Grammar* grammar){
        switch(stmt->getType()){
            case TYPE::Copy:
                return transfer_copy(in, stmt, grammar);
            case TYPE::Load:
                return transfer_load(in, stmt, grammar);
            case TYPE::Store:
                return transfer_store(in, stmt, grammar);
            case TYPE::Address:
                return transfer_address(in, stmt, grammar);
            default:
                return NULL;
        }
    }


    PEGraph* transfer_copy(PEGraph* in, Stmt* stmt,Grammar* grammar);

    PEGraph* transfer_load(PEGraph* in, Stmt* stmt,Grammar* grammar);

    PEGraph* transfer_store(PEGraph* in, Stmt* stmt,Grammar* grammar);

    PEGraph* transfer_address(PEGraph* in, Stmt* stmt,Grammar* grammar);

    static bool is_strong_update(vertexid_t x,PEGraph *out,Grammar *grammar);

    void strong_update(vertexid_t x,PEGraph *out,std::set<vertexid_t> &vertices,Grammar *grammar);

    void must_alias(vertexid_t x,PEGraph *out,std::set<vertexid_t> &vertices,Grammar *grammar);

    bool isDirectAssignEdges(vertexid_t src,vertexid_t dst,label_t label,std::set<vertexid_t> &vertices,Grammar *grammar);

    void peg_compute_add(PEGraph *out,Stmt* stmt,Grammar* grammar);

    void peg_compute_delete(PEGraph *out,Grammar* grammar, std::unordered_map<int, EdgesToDelete*>& m);

//    void initComputationSet_add(ComputationSet &compset,PEGraph *out,Stmt *stmt);
//
//    void initComputationSet_delete(ComputationSet &compset,PEGraph *out, std::unordered_map<int, EdgesToDelete*>& m);

    void findDeletedEdge(EdgesToDelete* edgesToDelete, int src, std::set<vertexid_t> &vertices);
};


#endif /* COMP_CFG_COMPUTE_H_ */
