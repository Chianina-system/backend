/*
 * engine.cpp
 *
 *  Created on: Jun 6, 2019
 *      Author: zqzuo
 */

#include <iostream>
#include "comp/cfg_compute.h"

using namespace std;



int main() {
	CFG *cfg = new CFG_map();
	GraphStore *graphstore = new NaiveGraphStore();
//	Partition *partition = nullptr;
//	cout << cfg << endl;

	CFGCompute::load("/home/zqzuo/Desktop/inlined/final", "/home/zqzuo/Desktop/inlined/id_stmt_info.txt", "/home/zqzuo/Desktop/inlined/var_singleton_info.txt", cfg, graphstore);
	CFGCompute::do_worklist(cfg, graphstore);

	delete cfg;
	delete graphstore;

//	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
