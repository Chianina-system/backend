#ifndef COMP_CFG_MAP_OUTCORE_H_
#define COMP_CFG_MAP_OUTCORE_H_

#include "cfg.h"
#include "../utility/FileUtil.hpp"
//#include "cfg_node_cache.h"

using namespace std;

class CFG_map_outcore : public CFG{

public:
    CFG_map_outcore(){}

    virtual ~CFG_map_outcore(){
        //        for (auto &node : nodes) {
        //            delete node;
        //        }
    }

    inline bool isMirror(CFGNode* node){

        return mirrors.find(node) != mirrors.end();
    }

    inline std::unordered_set<CFGNode*>& getActiveNodes(){

        return actives;
    }

    //void print(std::ostream& str) const override;

    virtual void loadCFG_ooc(bool file_mode, const string& file_cfg, const string& file_stmt, const string& file_mirrors_call, const string& file_mirrors_shallow, const string& file_actives) = 0;

   



    void addOneNode(CFGNode *Node)  {
        nodes.push_back(Node);
    }

    void addOnePred(CFGNode *succ, CFGNode *pred)  {
		if(predes.find(succ) == predes.end()) {
			predes[succ] = std::vector<CFGNode*>();
		}
		predes[succ].push_back(pred);
    }

    void addOneSucc(CFGNode *pred, CFGNode *succ)  {
    	if(succes.find(pred) == succes.end()){
    		succes[pred] = std::vector<CFGNode*>();
    	}
		succes[pred].push_back(succ);
		this->number_edges++;
    }


   void print(std::ostream& str) const override {
    	str << "The number of nodes in CFG: \t" <<"\n";
    	//str << "The number of edges in CFG: \t" << "\n";
    }
protected:
    
    std::unordered_set<CFGNode*> mirrors;

    std::unordered_set<CFGNode*> actives;


};


#endif /* COMP_CFG_MAP_OUTCORE_H_ */
