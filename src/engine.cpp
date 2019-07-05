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
	Singletons * singletons = new Singletons();
    Grammar *grammar = new Grammar();

//	Partition *partition = nullptr;
//	cout << cfg << endl;

	CFGCompute::load("/home/zqzuo/Desktop/inlined/final", "/home/zqzuo/Desktop/inlined/id_stmt_info.txt", cfg,
			"/home/zqzuo/Desktop/inlined/var_singleton_info.txt", singletons, graphstore, "/home/zqzuo/Desktop/inlined/rules_pointsto.txt", grammar);
	CFGCompute::do_worklist_asynchronous(cfg, graphstore, grammar, singletons);

	delete cfg;
	delete graphstore;
	delete grammar;

//	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
