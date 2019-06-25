/*
 * engine.cpp
 *
 *  Created on: Jun 6, 2019
 *      Author: zqzuo
 */

#include <iostream>
#include "comp/cfg_compute.h"
#include "comp/art.h"

using namespace std;



int main() {
	CFG *cfg = nullptr;
	GraphStore *graphstore = new ART();
	Partition *partition = nullptr;

	CFGCompute* computer = new CFGCompute();
	computer->load(partition, cfg, graphstore);
	computer->do_worklist(cfg, graphstore);             //get the pegraph using the graphstore

	delete cfg;
	delete graphstore;

	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
