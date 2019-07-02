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
#include "naive_graphstore.h"
//#include "concurrent_worklist.h"
#include "concurrent_workset.h"
#include "concurrent_workqueue.h"
#include "grammar.h"
#include "peg_compute.h"
#include "computationset.h"
using namespace std;

class CFGCompute{

public:
	static const int level_log = 0;

	static const int level_log_1 = 1;

	static const unsigned int num_threads = 4;

    static bool load(Partition* part, CFG* cfg, GraphStore* graphstore);

    static bool load(const string& file_cfg, const string& file_stmt, CFG *cfg, const string& file_singleton, GraphStore *graphstore, const string& file_grammar, Grammar * grammar);

    static void do_worklist(CFG* cfg, GraphStore* graphstore, Grammar* grammar); //worklist algorithm in parallel



private:
    static void compute(CFG* cfg, GraphStore* graphstore, Concurrent_Worklist* worklist_1, Concurrent_Worklist* worklist_2, Grammar* grammar);

    static PEGraph* combine(GraphStore* graphstore, std::vector<CFGNode*>& preds);

    static PEGraph* transfer(PEGraph* in, Stmt* stmt,Grammar* grammar, GraphStore* graphstore){
        switch(stmt->getType()){
            case TYPE::Assign:
                return transfer_copy(in, stmt, grammar, graphstore);
            case TYPE::Load:
                return transfer_load(in, stmt, grammar, graphstore);
            case TYPE::Store:
                return transfer_store(in, stmt, grammar, graphstore);
            case TYPE::Alloca:
                return transfer_address(in, stmt, grammar, graphstore);
            default:
                return nullptr;
        }
    }

    static PEGraph* transfer_copy(PEGraph* in, Stmt* stmt,Grammar* grammar, GraphStore* graphstore);

    static PEGraph* transfer_load(PEGraph* in, Stmt* stmt,Grammar* grammar, GraphStore* graphstore);

    static PEGraph* transfer_store(PEGraph* in, Stmt* stmt,Grammar* grammar, GraphStore* graphstore);

    static PEGraph* transfer_address(PEGraph* in, Stmt* stmt,Grammar* grammar, GraphStore* graphstore);

    static bool is_strong_update(vertexid_t x,PEGraph *out,Grammar *grammar, GraphStore* graphstore);

    static void strong_update(vertexid_t x,PEGraph *out,std::set<vertexid_t> &vertices,Grammar *grammar,std::set<vertexid_t> &vertices_delete, GraphStore* graphstore);

    static void must_alias(vertexid_t x,PEGraph *out,std::set<vertexid_t> &vertices,Grammar *grammar, std::set<vertexid_t> &vertices_delete, GraphStore* graphstore);

    static bool isDirectAssignEdges(vertexid_t src,vertexid_t dst,label_t label,std::set<vertexid_t> &vertices,Grammar *grammar);

    static void peg_compute_add(PEGraph *out,Stmt* stmt,Grammar* grammar);

    static void peg_compute_delete(PEGraph *out,Grammar* grammar, std::unordered_map<vertexid_t, EdgeArray>& m);
	static void getDirectAssignEdges(PEGraph* out, std::set<vertexid_t>& vertices_changed, Grammar* grammar, std::unordered_map<vertexid_t, EdgeArray>& m);
	static void getDirectAddedEdges(PEGraph *out, Stmt *stmt, Grammar *grammar, std::unordered_map<vertexid_t, EdgeArray>& m);
	static void removeExistingEdges(const EdgeArray& edges_src, vertexid_t src,
			PEGraph* out, std::unordered_map<vertexid_t, EdgeArray>& m);
};


#endif /* COMP_CFG_COMPUTE_H_ */
