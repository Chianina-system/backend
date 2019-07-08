/*
 * cfg_map_outcore.h
 *
 *  Created on: Jul 8, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_MAP_OUTCORE_H_
#define COMP_CFG_MAP_OUTCORE_H_

#include "cfg_map.h"

class CFG_map_outcore : public CFG_map {


public:
	CFG_map_outcore(){}

	~CFG_map_outcore(){}

	inline bool isMirror(CFGNode* node){
		return in_mirrors.find(node) != in_mirrors.end() || out_mirrors.find(node) != out_mirrors.end();
	}

	inline bool isInMirror(CFGNode* node){
		return in_mirrors.find(node) != in_mirrors.end();
	}

	inline bool isOutMirror(CFGNode* node){
		return out_mirrors.find(node) != out_mirrors.end();
	}

	void loadCFG(const string& file_cfg, const string& file_stmt, const string& file_mirrors) override {
//		CFG_map::loadCFG(file_cfg, file_stmt);

	}



private:
	std::unordered_set<CFGNode*> in_mirrors;

	std::unordered_set<CFGNode*> out_mirrors;



};



#endif /* COMP_CFG_MAP_OUTCORE_H_ */
