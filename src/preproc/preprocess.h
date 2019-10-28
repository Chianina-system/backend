/*
 * preprocess.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef PREPROC_PREPROCESS_H_
#define PREPROC_PREPROCESS_H_

#include "../common/CommonLibs.hpp"
#include "../comp/cfg_node.h"
#include "../comp/context.h"


class Preprocess{

private:




public:
//	Preprocess(){
//
//	}


	static void write_to_partition_cfg(Partition partition, string& line){
		string cfg_partition_file = Context::file_cfg + to_string(partition);
		ofstream myfile;
		myfile.open(cfg_partition_file, std::ofstream::out | std::ofstream::app);
		if (myfile.is_open()){
			myfile << line << "\n";
			myfile.close();
		}
	}

	static void write_to_partition_stmt(Partition partition, string& line){
		string stmt_partition_file = Context::file_stmts + to_string(partition);
		ofstream myfile;
		myfile.open(stmt_partition_file, std::ofstream::out | std::ofstream::app);
		if (myfile.is_open()){
			myfile << line << "\n";
			myfile.close();
		}
	}

	static void write_to_partition_mirrors_in(string& src_id, Partition dst_p){
		string stmt_partition_file = Context::folder_mirrors_in + to_string(dst_p);
		ofstream myfile;
		myfile.open(stmt_partition_file, std::ofstream::out | std::ofstream::app);
		if (myfile.is_open()){
			myfile << src_id << "\n";
			myfile.close();
		}
	}

	static void write_to_partition_mirrors_out(string& dst_id, Partition src_p){
		string stmt_partition_file = Context::folder_mirrors_out + to_string(src_p);
		ofstream myfile;
		myfile.open(stmt_partition_file, std::ofstream::out | std::ofstream::app);
		if (myfile.is_open()){
			myfile << dst_id << "\n";
			myfile.close();
		}
	}

	static void write_to_partition_actives(Partition part, vertexid_t id){
		string stmt_partition_file = Context::folder_actives + to_string(part);
		ofstream myfile;
		myfile.open(stmt_partition_file, std::ofstream::out | std::ofstream::app);
		if (myfile.is_open()){
			myfile << id << "\n";
			myfile.close();
		}
	}

	static void write_to_partition_cfg_binary(Partition partition, vertexid_t src_id, vertexid_t dst_id, Buffer& buf){
		memcpy(buf.getData() + buf.getSize(), (char*)& src_id, sizeof(vertexid_t));
		buf.setSize(buf.getSize() + sizeof(vertexid_t));
		memcpy(buf.getData() + buf.getSize(), (char*)& dst_id, sizeof(vertexid_t));
		buf.setSize(buf.getSize() + sizeof(vertexid_t));

		if(buf.isFull()){
			string cfg_partition_file = Context::file_cfg + to_string(partition);
			FILE *f_cfg = fopen(cfg_partition_file.c_str(), "ab");
			assert(f_cfg);
			fwrite(buf.getData(), buf.getSize(), 1, f_cfg);
			buf.setSize(0);
			fclose(f_cfg);
		}
	}

	static void write_to_partition_stmt_binary(Partition partition, CFGNode* node, Buffer& buf){
		size_t s = node->get_size_bytes();
		assert(s + sizeof(size_t) <= buf.getCapacity());
		if(buf.getSize() + s + sizeof(size_t) > buf.getCapacity()){
			string stmt_partition_file = Context::file_stmts + to_string(partition);
			FILE *f_stmt = fopen(stmt_partition_file.c_str(), "ab");
			assert(f_stmt);
			fwrite(buf.getData(), buf.getSize(), 1, f_stmt);
			buf.setSize(0);
			fclose(f_stmt);
		}

		node->write_to_buf(buf);
	}

	static void write_to_partition_mirrors_in_binary(vertexid_t src_id, Partition dst_p, Buffer& buf){
		memcpy(buf.getData() + buf.getSize(), (char*)& src_id, sizeof(vertexid_t));
		buf.setSize(buf.getSize() + sizeof(vertexid_t));

		if(buf.isFull()){
			string mirrors_in_file = Context::folder_mirrors_in + to_string(dst_p);
			FILE *f_mirrors_in = fopen(mirrors_in_file.c_str(), "ab");
			assert(f_mirrors_in);
			fwrite(buf.getData(), buf.getSize(), 1, f_mirrors_in);
			buf.setSize(0);
			fclose(f_mirrors_in);
		}
	}

	static void write_to_partition_mirrors_out_binary(vertexid_t dst_id, Partition src_p, Buffer& buf){
		memcpy(buf.getData() + buf.getSize(), (char*)& dst_id, sizeof(vertexid_t));
		buf.setSize(buf.getSize() + sizeof(vertexid_t));

		if(buf.isFull()){
			string mirrors_out_file = Context::folder_mirrors_out + to_string(src_p);
			FILE *f_mirrors_out = fopen(mirrors_out_file.c_str(), "ab");
			assert(f_mirrors_out);
			fwrite(buf.getData(), buf.getSize(), 1, f_mirrors_out);
			buf.setSize(0);
			fclose(f_mirrors_out);
		}
	}

	static void write_to_partition_actives_binary(Partition part, vertexid_t id, Buffer& buf){
		memcpy(buf.getData() + buf.getSize(), (char*)& id, sizeof(vertexid_t));
		buf.setSize(buf.getSize() + sizeof(vertexid_t));

		if(buf.isFull()){
			string actives_file = Context::folder_actives + to_string(part);
			FILE *f_actives = fopen(actives_file.c_str(), "ab");
			assert(f_actives);
			fwrite(buf.getData(), buf.getSize(), 1, f_actives);
			buf.setSize(0);
			fclose(f_actives);
		}
	}

	static string nextItem(istringstream &lineStream) {
    	string item;
    	getline(lineStream, item, '\t');
    	return item;
	}

	static int getMaxId(string filePath) {
    	int maxId = 0;
    	ifstream fin;
    	fin.open(filePath.c_str());
    	string line;
    	while (getline(fin, line)) {
        	istringstream lineStream(line);
        	int src = atoi(nextItem(lineStream).c_str());
        	if (src > maxId)
            	maxId = src;
        	int dst = atoi(nextItem(lineStream).c_str());
        	if (dst > maxId)
            	maxId = dst;
    	}
    	fin.close();
    	return maxId;
	}

	static void readEdges(string filePath, vector<int> &mark, map<int, vector<int>> &inGraph, map<int, vector<int>> &outGraph) {
    	ifstream fin;
    	fin.open(filePath.c_str());
    	string line;
    	while (getline(fin, line)) {
        	istringstream lineStream(line);
        	int src = atoi(nextItem(lineStream).c_str());
        	mark[src] = 0;
        	int dst = atoi(nextItem(lineStream).c_str());
        	mark[dst] = 0;
			outGraph[src].push_back(dst);
			inGraph[dst].push_back(src);
    	}
    	fin.close();
	}

	static void readEntryNodes(string filePath, queue<int> &entryNodes) {
    	ifstream fin;
    	fin.open(filePath.c_str());
    	string line;
    	while (getline(fin, line)) {
        	entryNodes.push(atoi(line.c_str()));
    	}
    	fin.close();
	}

	static void bfs(int k, int numPartitionNodes, vector<int> &mark, map<int, vector<int>> &inGraph, map<int, vector<int>> &outGraph, queue<int> &entryNodes, Context& context) {
    	vector<int> partitionNodes;
    
    	while (numPartitionNodes>0) {
    		int startNode = -1;
    		while (!entryNodes.empty()) {
        		int tmp = entryNodes.front();
        		entryNodes.pop();
        		if (mark[tmp]==0) {
            		startNode=tmp;
            		break;
        		}
    		}
    		if (startNode==-1) {
        		for (unsigned int i=0; i<mark.size(); i++) {
            		if (mark[i]==0) {
                		startNode=i;
                		break;
            		}
        		}
    		}
    
    		queue<int> q;
    		partitionNodes.push_back(startNode);
    		numPartitionNodes--;
    		mark[startNode] = 1;
    		q.push(startNode);
    		while (!q.empty() && numPartitionNodes>0) {
        		int tmp = q.front();
        		q.pop();
				auto iterIn = inGraph.find(tmp);
				if (iterIn!=inGraph.end()) {
					for (unsigned int i=0; i<(iterIn->second).size(); i++) {
						int inNodeId = (iterIn->second)[i];
						if (mark[inNodeId]==0 && numPartitionNodes>0) {
                			partitionNodes.push_back(inNodeId);
                			numPartitionNodes--;
                			mark[inNodeId] = 1;
                			q.push(inNodeId);
            			}
					}
				}
				auto iterOut = outGraph.find(tmp);
				if (iterOut!=outGraph.end()) {
					for (unsigned int i=0; i<(iterOut->second).size(); i++) {
						int outNodeId = (iterOut->second)[i];
						if (mark[outNodeId]==0 && numPartitionNodes>0) {
                			partitionNodes.push_back(outNodeId);
                			numPartitionNodes--;
                			mark[outNodeId] = 1;
                			q.push(outNodeId);
            			}
					}
				}
    		}
    	}
    
		context.setPartitionInfo(k, partitionNodes);
	}

	static void createPartition(Context& context) {
		int maxId = getMaxId(context.getFileCfg());
    	// -1: not exists
    	vector<int> mark(maxId+1,-1);
		map<int, vector<int>> inGraph;
		map<int, vector<int>> outGraph;
    	readEdges(context.getFileCfg(), mark, inGraph, outGraph);
    	int numNodes = 0;
    	for (unsigned int i=0; i<mark.size(); i++) {
        	if (mark[i]==0)
            	numNodes++;
    	}
    	int numPartitions = context.getNumberPartitions();
    	queue<int> entryNodes;
    	readEntryNodes(context.getFileEntries(), entryNodes);
    
    	int tmp = numNodes/numPartitions;
    	for (int i=0; i<numPartitions; i++) {
        	if (i==numPartitions-1)
            	bfs(i, tmp+numNodes%numPartitions, mark, inGraph, outGraph, entryNodes, context);
        	else
            	bfs(i, tmp, mark, inGraph, outGraph, entryNodes, context);
    	}
	}

	static void process(Context& context, bool file_mode){
		createPartition(context);

		//for debugging
		Logger::print_thread_info_locked("process starting...\n", LEVEL_LOG_FUNCTION);

		//clear the folder for storing the tmp data
		FileUtil::reset_folder(Context::working_path);

		if(file_mode){
			//deal with cfg file
			ifstream myfile_cfg(context.getFileCfg());
			if (myfile_cfg.is_open()) {
				string line;
				while (getline(myfile_cfg, line)) {
					if(line == ""){
						continue;
					}

					std::stringstream stream(line);
					std::string src_id, dst_id;
					stream >> src_id >> dst_id;
					Partition src_p = context.getPartition(atoi(src_id.c_str()));
					Partition dst_p = context.getPartition(atoi(dst_id.c_str()));
					//write to the respectively cfg files
					if(src_p == dst_p){
						write_to_partition_cfg(src_p, line);
					}
					else{
						write_to_partition_cfg(src_p, line);
						write_to_partition_cfg(dst_p, line);

						//deal with mirrors
						write_to_partition_mirrors_in(src_id, dst_p);
						write_to_partition_mirrors_out(dst_id, src_p);
					}
				}
				myfile_cfg.close();
			}

			//deal with stmt file
			ifstream myfile_stmt(context.getFileStmts());
			if (myfile_stmt.is_open()) {
				string line;
				while (getline(myfile_stmt, line)) {
					if(line == ""){
						continue;
					}

					std::stringstream stream(line);
					std::string id;
					stream >> id;
					vertexid_t node_id = atoi(id.c_str());

					Partition p = context.getPartition(node_id);
					//write to the respectively stmt files
					write_to_partition_stmt(p, line);

	//				//initialize graphs by adding self-loop edges
	//				Stmt* stmt = new Stmt(stream);
	//			    vertexid_t src = stmt->getSrc();
	//			    EdgeArray edges_src = EdgeArray();
	//
	//			    vertexid_t dst = stmt->getDst();
	//			    EdgeArray edges_dst = EdgeArray();
	//
	//			    vertexid_t aux = stmt->getAux();
	//			    EdgeArray edges_aux = EdgeArray();
	//
	//				//self-loop edges
	//				for (int i = 0; i < context.getGrammar()->getNumErules(); ++i) {
	//					char label = context.getGrammar()->getErule(i);
	//					edges_src.addOneEdge(src, label);
	//					edges_dst.addOneEdge(dst, label);
	//					if (stmt->isValidAux()) {
	//						edges_aux.addOneEdge(aux, label);
	//					}
	//				}
	//
	//			    //merge and sort
	//			    edges_src.merge();
	//			    edges_dst.merge();
	//			    if(stmt->isValidAux()){
	//					edges_aux.merge();
	//			    }
	//
	//			    PEGraph* pegraph = new PEGraph();
	//			    pegraph->setEdgeArray(src, edges_src);
	//			    pegraph->setEdgeArray(dst, edges_dst);
	//			    if(stmt->isValidAux()){
	//			    	pegraph->setEdgeArray(aux, edges_aux);
	//			    }
	//
	//			    //write to file
	//				const string file_graphs_in = Context::folder_graphs_in + std::to_string(p);
	//				if(readable){
	//					ofstream myfile;
	//					myfile.open(file_graphs_in, std::ofstream::out | std::ofstream::app);
	//					if (myfile.is_open()){
	//						//write a pegraph into file
	//					    myfile << node_id << "\t";
	//					    pegraph->write_readable(myfile);
	//					    myfile << "\n";
	//
	//						myfile.close();
	//					}
	//				}
	//				else{
	//
	//				}
	//
	//				//clean
	//				delete stmt;
	//				delete pegraph;

				}
				myfile_stmt.close();
			}

			//initialize active nodes
			ifstream myfile_entries(context.getFileEntries());
			if (myfile_entries.is_open()) {
				string line;
				while (getline(myfile_entries, line)) {
					if(line == ""){
						continue;
					}

					vertexid_t id = atoi(line.c_str());
					Partition part = context.getPartition(id);
					write_to_partition_actives(part, id);

					//update entry partitions according to entry nodes
	//				if(part > context.getNumberPartitions()){
	//					cout << id << endl;
	//				}
					context.update_priority(part, 1);
				}

				myfile_entries.close();
			}

		}
		else{//binary format
			//deal with cfg file
//			cout << "deal with cfg file..." << endl;
			ifstream myfile_cfg(context.getFileCfg());
			if (myfile_cfg.is_open()) {
				unsigned int num_partitions = context.getNumberPartitions();
				Buffer buffers[num_partitions];
				Buffer buffers_mirrors_in[num_partitions];
				Buffer buffers_mirrors_out[num_partitions];

				string line;
				while (getline(myfile_cfg, line)) {
					if(line == ""){
						continue;
					}

					std::stringstream stream(line);
					std::string src_Id, dst_Id;
					stream >> src_Id >> dst_Id;
					vertexid_t src_id = atoi(src_Id.c_str());
					vertexid_t dst_id = atoi(dst_Id.c_str());
					Partition src_p = context.getPartition(src_id);
					Partition dst_p = context.getPartition(dst_id);
					//write to the respectively cfg files
					if(src_p == dst_p){
						write_to_partition_cfg_binary(src_p, src_id, dst_id, buffers[src_p]);
					}
					else{
						write_to_partition_cfg_binary(src_p, src_id, dst_id, buffers[src_p]);
						write_to_partition_cfg_binary(dst_p, src_id, dst_id, buffers[dst_p]);

						//deal with mirrors
						write_to_partition_mirrors_in_binary(src_id, dst_p, buffers_mirrors_in[dst_p]);
						write_to_partition_mirrors_out_binary(dst_id, src_p, buffers_mirrors_out[src_p]);
					}
				}

				//write the remaining buf to cfg_file and mirrors_file
				for(unsigned int i = 0; i < num_partitions; ++i){
					if(!buffers[i].isEmpty()){
						string cfg_partition_file = Context::file_cfg + to_string(i);
						FILE *f_cfg = fopen(cfg_partition_file.c_str(), "ab");
						assert(f_cfg);
						fwrite(buffers[i].getData(), buffers[i].getSize(), 1, f_cfg);
						buffers[i].setSize(0);
						fclose(f_cfg);
					}

					if(!buffers_mirrors_in[i].isEmpty()){
						string mirrors_in_file = Context::folder_mirrors_in + to_string(i);
						FILE *f_mirrors_in = fopen(mirrors_in_file.c_str(), "ab");
						assert(f_mirrors_in);
						fwrite(buffers_mirrors_in[i].getData(), buffers_mirrors_in[i].getSize(), 1, f_mirrors_in);
						buffers_mirrors_in[i].setSize(0);
						fclose(f_mirrors_in);
					}

					if(!buffers_mirrors_out[i].isEmpty()){
						string mirrors_out_file = Context::folder_mirrors_out + to_string(i);
						FILE *f_mirrors_out = fopen(mirrors_out_file.c_str(), "ab");
						assert(f_mirrors_out);
						fwrite(buffers_mirrors_out[i].getData(), buffers_mirrors_out[i].getSize(), 1, f_mirrors_out);
						buffers_mirrors_out[i].setSize(0);
						fclose(f_mirrors_out);
					}
				}

				myfile_cfg.close();
			}
//			cout << "cfg done" << endl;


			//deal with stmt file
			ifstream myfile_stmt(context.getFileStmts());
			if (myfile_stmt.is_open()) {
				unsigned int num_partitions = context.getNumberPartitions();
				Buffer buffers_stmts[num_partitions];

				string line;
				while (getline(myfile_stmt, line)) {
					if(line == ""){
						continue;
					}

					CFGNode* cfgNode = new CFGNode(line);

					Partition p = context.getPartition(cfgNode->getCfgNodeId());
					//write to the respectively stmt files
					write_to_partition_stmt_binary(p, cfgNode, buffers_stmts[p]);

					delete cfgNode;
				}

				//write the remaining to stmts_file
				for(unsigned int i = 0; i < num_partitions; ++i){
					if (!buffers_stmts[i].isEmpty()) {
						string stmts_file = Context::file_stmts + to_string(i);
						FILE *f_stmts = fopen(stmts_file.c_str(), "ab");
						assert(f_stmts);
						fwrite(buffers_stmts[i].getData(), buffers_stmts[i].getSize(), 1, f_stmts);
						buffers_stmts[i].setSize(0);
						fclose(f_stmts);
					}
				}

				myfile_stmt.close();
			}
//			cout << "stmt file done" << endl;


			//initialize active nodes
			ifstream myfile_entries(context.getFileEntries());
			if (myfile_entries.is_open()) {
				unsigned int num_partitions = context.getNumberPartitions();
				Buffer buffers_actives[num_partitions];

				string line;
				while (getline(myfile_entries, line)) {
					if(line == ""){
						continue;
					}

					vertexid_t id = atoi(line.c_str());
					Partition part = context.getPartition(id);
					write_to_partition_actives_binary(part, id, buffers_actives[part]);

					//update entry partitions according to entry nodes
					context.update_priority(part, 1);
				}

				//write the remaining to actives_file
				for(unsigned int i = 0; i < num_partitions; ++i){
					if (!buffers_actives[i].isEmpty()) {
						string actives_file = Context::folder_actives + to_string(i);
						FILE *f_actives = fopen(actives_file.c_str(), "ab");
						assert(f_actives);
						fwrite(buffers_actives[i].getData(), buffers_actives[i].getSize(), 1, f_actives);
						buffers_actives[i].setSize(0);
						fclose(f_actives);
					}
				}
				myfile_entries.close();
			}

		}

		//for debugging
		Logger::print_thread_info_locked("process finished.\n", LEVEL_LOG_FUNCTION);
	}



};



#endif /* PREPROC_PREPROCESS_H_ */
