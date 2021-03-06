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

#include "scc_tarjan.hpp"


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

	static void write_to_partition_mirrors_call(string& stmt, Partition part){
		string stmt_partition_file = Context::folder_mirrors_call + to_string(part);
		ofstream myfile;
		myfile.open(stmt_partition_file, std::ofstream::out | std::ofstream::app);
		if (myfile.is_open()){
			myfile << stmt << "\n";
			myfile.close();
		}
	}

	static void write_to_partition_mirrors_shallow(string& id, Partition part){
		string stmt_partition_file = Context::folder_mirrors_shallow + to_string(part);
		ofstream myfile;
		myfile.open(stmt_partition_file, std::ofstream::out | std::ofstream::app);
		if (myfile.is_open()){
			myfile << id << "\n";
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

	static void write_to_partition_mirrors_call_binary(CFGNode* node, Partition part, Buffer& buf){
		size_t s = node->get_size_bytes();
		assert(s + sizeof(size_t) <= buf.getCapacity());
		if(buf.getSize() + s + sizeof(size_t) > buf.getCapacity()){
			string mirrors_call_file = Context::folder_mirrors_call + to_string(part);
			FILE *f_mirrors_call = fopen(mirrors_call_file.c_str(), "ab");
			assert(f_mirrors_call);
			fwrite(buf.getData(), buf.getSize(), 1, f_mirrors_call);
			buf.setSize(0);
			fclose(f_mirrors_call);
		}

		node->write_to_buf(buf);
	}

	static void write_to_partition_mirrors_shallow_binary(vertexid_t id, Partition part, Buffer& buf){
		memcpy(buf.getData() + buf.getSize(), (char*)& id, sizeof(vertexid_t));
		buf.setSize(buf.getSize() + sizeof(vertexid_t));

		if(buf.isFull()){
			string mirrors_shallow_file = Context::folder_mirrors_shallow + to_string(part);
			FILE *f_mirrors_shallow = fopen(mirrors_shallow_file.c_str(), "ab");
			assert(f_mirrors_shallow);
			fwrite(buf.getData(), buf.getSize(), 1, f_mirrors_shallow);
			buf.setSize(0);
			fclose(f_mirrors_shallow);
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

	static void createPartitionBfs(Context& context) {
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

	static void readEdges(string filePath, vector<bool> &exist, vector<vector<int>> &graph) {
    	ifstream fin;
    	fin.open(filePath.c_str());
    	string line;
    	while (getline(fin, line)) {
        	istringstream lineStream(line);
        	int src = atoi(nextItem(lineStream).c_str());
        	exist[src] = true;
        	int dst = atoi(nextItem(lineStream).c_str());
        	exist[dst] = true;
        	graph[src].push_back(dst);
    	}
    	fin.close();
	}

	static void topSort(vector<vector<int>> &graph, vector<int> &inDegree, vector<int> &res) {
    	deque<int> q;
    	for (int i=0; i<inDegree.size(); i++)
        	if (inDegree[i]==0)
            	q.push_back(i);

    	while (!q.empty()) {
        	int from = q.front();
        	q.pop_front();
        	res.push_back(from);
        	for (int i=0; i<graph[from].size(); i++) {
            	int to = graph[from][i];
            	if (--inDegree[to]==0) {
                	q.push_front(to);
            	}
        	}
        
        	if (q.empty() && res.size()<inDegree.size()) {
            	int tmp=-1;
            	for (int i=res.size()-1; i>=0; i--) {
                	for (int j=0; j<graph[res[i]].size(); j++) {
                    	if (inDegree[graph[res[i]][j]]>0) {
                        	tmp = graph[res[i]][j];
                        	break;
                    	}
                	}
                	if (tmp!=-1)
                    	break;
            	}
            	if (tmp==-1) {
                	for (int i=0; i<inDegree.size(); i++) {
                    	if (inDegree[i]>0) {
                        	tmp = i;
                        	break;
                    	}
                	}
            	}
            	inDegree[tmp]=0;
            	q.push_back(tmp);
        	}
    	}
	}

	static void createPartitionTopSortHelper(int k, int numPartitionNodes, int &idx, vector<int> &sortedNodes, vector<bool> &exist, Context& context) {
		vector<int> partitionNodes;
		while (idx<sortedNodes.size() && numPartitionNodes>0) {
			if (exist[sortedNodes[idx]]) {
				partitionNodes.push_back(sortedNodes[idx]);
				numPartitionNodes--;
			}
			idx++;
		}
		context.setPartitionInfo(k, partitionNodes);
	}

	static void createPartitionTopSort(Context& context) {
		int maxId = getMaxId(context.getFileCfg());
    	vector<bool> exist(maxId+1,false);
    	vector<vector<int>> graph(maxId+1);
    	readEdges(context.getFileCfg(), exist, graph);
    	int numNodes = 0;
    	for (int i=0; i<exist.size(); i++) {
        	if (exist[i])
            	numNodes++;
    	}
    	vector<int> inDegree(maxId+1,0);
    	for (int i=0; i<graph.size(); i++) {
        	for (int j=0; j<graph[i].size(); j++)
            	inDegree[graph[i][j]]++;
    	}
    	int numPartitions = context.getNumberPartitions();
    
    	vector<int> sortedNodes;
    	topSort(graph, inDegree, sortedNodes);

		int tmp = numNodes/numPartitions;
		int idx = 0;
    	for (int i=0; i<numPartitions; i++) {
        	if (i==numPartitions-1)
            	createPartitionTopSortHelper(i, tmp+numNodes%numPartitions, idx, sortedNodes, exist, context);
        	else
            	createPartitionTopSortHelper(i, tmp, idx, sortedNodes, exist, context);
    	}
	}

	static void createPartitions_scc(Context& context) {
    	int maxId = getMaxId(context.getFileCfg());
    	vector<bool> exist(maxId+1,false);
    	vector<vector<int>> graph(maxId+1);
    	readEdges(context.getFileCfg(), exist, graph);

    	Graph g(maxId+1);
    	for (int i=0; i<graph.size(); i++)
        	for (int j=0; j<graph[i].size(); j++)
            	g.addEdge(i, graph[i][j]);
    	g.SCC();

    	int condensationGraph_numNodes = g.sccs.size();
    	vector<vector<int>> condensationGraph(condensationGraph_numNodes);
//     	for (int i=0; i<condensationGraph_numNodes; i++) {
//         	for (int j=0; j<condensationGraph_numNodes; j++) {
//             	if (j!=i) {
//                 	bool b = false;
//                 	vector<int> fromNodes = g.sccs[i];
//                 	unordered_set<int> toNodes;
//                 	for (int k=0; k<g.sccs[j].size(); k++)
//                     	toNodes.insert(g.sccs[j][k]);
//                 	for (int k=0; k<fromNodes.size(); k++) {
//                     	int fromNode = fromNodes[k];
//                     	for (int ki=0; ki<graph[fromNode].size(); ki++)
//                         	if (toNodes.find(graph[fromNode][ki]) != toNodes.end()) {
// 					b = true; break;
// 				}
// 			if (b==true) break;
//                 	}
//                 	if (b==true)
//                     	condensationGraph[i].push_back(j);
//             	}
//         	}
//     	}
	vector<int> belong(maxId+1);
    	for (int i=0; i<g.sccs.size(); i++)
        	for (int j=0; j<g.sccs[i].size(); j++)
            		belong[g.sccs[i][j]]=i;
    	vector<unordered_set<int>> condensationGraph_b(condensationGraph_numNodes);
    	for (int i=0; i<graph.size(); i++) {
        	for (int j=0; j<graph[i].size(); j++) {
            		int fb=belong[i];
            		int tb=belong[graph[i][j]];
            		condensationGraph_b[fb].insert(tb);
        	}
    	}
    	for (int i=0; i<condensationGraph_b.size(); i++)
        	for (auto it=condensationGraph_b[i].begin(); it!=condensationGraph_b[i].end(); it++)
            		if (i!=*it)
                		condensationGraph[i].push_back(*it);
    	vector<int> inDegree(condensationGraph_numNodes,0);
    	for (int i=0; i<condensationGraph.size(); i++) {
        	for (int j=0; j<condensationGraph[i].size(); j++)
            	inDegree[condensationGraph[i][j]]++;
    	}

    	vector<int> sortedNodes;
    	topSort(condensationGraph, inDegree, sortedNodes);

    	int numNodes = 0;
    	for (int i=0; i<exist.size(); i++) {
        	if (exist[i])
            	numNodes++;
    	}
    	int numPartitions = context.getNumberPartitions();
    	int avgNumPartitionNodes = numNodes/numPartitions;
    	int i=0;
    	int sum=0;
    	vector<int> partitionNodes;
    	for (int j=0; j<sortedNodes.size(); j++) {
        	if (g.sccs[sortedNodes[j]].size()==1 && exist[g.sccs[sortedNodes[j]][0]]==false)
            	continue;
        	sum+=g.sccs[sortedNodes[j]].size();
        	if (sum > avgNumPartitionNodes) {
            	context.setPartitionInfo(i, partitionNodes);
            	i++;
            	partitionNodes.clear();
            	sum=0;
            	j--;
            	if (i==numPartitions-1) {
                	j++;
                	for (; j<sortedNodes.size(); j++) {
                    	if (g.sccs[sortedNodes[j]].size()==1 && exist[g.sccs[sortedNodes[j]][0]]==false)
                        	continue;
                    	for (int k=0; k<g.sccs[sortedNodes[j]].size(); k++)
                        	partitionNodes.push_back(g.sccs[sortedNodes[j]][k]);
                	}
                	context.setPartitionInfo(i, partitionNodes);
			i++;
			partitionNodes.clear();
            	}
        	} else {
            	for (int k=0; k<g.sccs[sortedNodes[j]].size(); k++)
                	partitionNodes.push_back(g.sccs[sortedNodes[j]][k]);
        	}
    	}
	if (partitionNodes.size() != 0) {
		context.setPartitionInfo(i, partitionNodes);
	}	
	}

	static void process(Context& context, bool file_mode){
		createPartitions_scc(context);

		//for debugging
		Logger::print_thread_info_locked("process starting...\n", LEVEL_LOG_FUNCTION);

		//clear the folder for storing the tmp data
		FileUtil::reset_folder(Context::working_path);

		//record the mirrors-in statements
		std::unordered_map<vertexid_t, unordered_set<Partition>> mirrors_map;

		if(file_mode){
			cout << "text read starting..." << endl;
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

						//deal with mirrors in
//						write_to_partition_mirrors_in(src_id, dst_p);
						vertexid_t src_id_int = atoi(src_id.c_str());
						if(mirrors_map.find(src_id_int) == mirrors_map.end()){
							mirrors_map[src_id_int] = unordered_set<Partition>();
						}
						mirrors_map[src_id_int].insert(dst_p);

						//deal with mirrors out
//						write_to_partition_mirrors_out(dst_id, src_p);
						vertexid_t dst_id_int = atoi(dst_id.c_str());
						if(mirrors_map.find(dst_id_int) == mirrors_map.end()){
							mirrors_map[dst_id_int] = unordered_set<Partition>();
						}
						mirrors_map[dst_id_int].insert(src_p);
					}
				}
				myfile_cfg.close();
			}
			cout << "text cfg done" << endl;

			//deal with stmt file
			ifstream myfile_stmt(context.getFileStmts());
			if (myfile_stmt.is_open()) {
				string line;
				while (getline(myfile_stmt, line)) {
					if(line == ""){
						continue;
					}

					std::stringstream stream(line);
					std::string id, type;
					stream >> id >> type;
					vertexid_t node_id = atoi(id.c_str());

					Partition p = context.getPartition(node_id);
					//write to the respectively stmt files
					write_to_partition_stmt(p, line);

					//deal with mirrors statements
					if(mirrors_map.find(node_id) != mirrors_map.end()){
						if (type == "call" || type == "callfptr" || type == "calleefptr" || type == "return") {
							for(auto& partition: mirrors_map[node_id]){
								write_to_partition_mirrors_call(line, partition);
							}
						}
						else {
							for(auto& partition: mirrors_map[node_id]){
								write_to_partition_mirrors_shallow(id, partition);
							}
						}
					}

				}
				myfile_stmt.close();
			}
			cout << "text stmt done" << endl;

			//initialize active nodes
			ifstream myfile_entries(context.getFileEntries());
			if (myfile_entries.is_open()) {
				string line;
				while (getline(myfile_entries, line)) {
					if(line == ""){
						continue;
					}

					vertexid_t id = atoi(line.c_str());
//					cout << "id: " << id << endl;
					Partition part = context.getPartition(id);
//					cout << "part: " << part << endl;
					write_to_partition_actives(part, id);

					//update entry partitions according to entry nodes
	//				if(part > context.getNumberPartitions()){
	//					cout << id << endl;
	//				}
					context.update_priority(part, 1);
				}

				myfile_entries.close();
			}
			cout << "text actives done" << endl;

		}
		else{//binary format
			cout << "binary read starting..." << endl;
			//deal with cfg file
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
//						write_to_partition_mirrors_in_binary(src_id, dst_p, buffers_mirrors_in[dst_p]);
						if(mirrors_map.find(src_id) == mirrors_map.end()){
							mirrors_map[src_id] = unordered_set<Partition>();
						}
						mirrors_map[src_id].insert(dst_p);

//						write_to_partition_mirrors_out_binary(dst_id, src_p, buffers_mirrors_out[src_p]);
						if(mirrors_map.find(dst_id) == mirrors_map.end()){
							mirrors_map[dst_id] = unordered_set<Partition>();
						}
						mirrors_map[dst_id].insert(src_p);
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

//					if(!buffers_mirrors_in[i].isEmpty()){
//						string mirrors_in_file = Context::folder_mirrors_call + to_string(i);
//						FILE *f_mirrors_in = fopen(mirrors_in_file.c_str(), "ab");
//						assert(f_mirrors_in);
//						fwrite(buffers_mirrors_in[i].getData(), buffers_mirrors_in[i].getSize(), 1, f_mirrors_in);
//						buffers_mirrors_in[i].setSize(0);
//						fclose(f_mirrors_in);
//					}
//
//					if(!buffers_mirrors_out[i].isEmpty()){
//						string mirrors_out_file = Context::folder_mirrors_shallow + to_string(i);
//						FILE *f_mirrors_out = fopen(mirrors_out_file.c_str(), "ab");
//						assert(f_mirrors_out);
//						fwrite(buffers_mirrors_out[i].getData(), buffers_mirrors_out[i].getSize(), 1, f_mirrors_out);
//						buffers_mirrors_out[i].setSize(0);
//						fclose(f_mirrors_out);
//					}
				}

				myfile_cfg.close();
			}
			cout << "binary cfg done" << endl;


			//deal with stmt file
			ifstream myfile_stmt(context.getFileStmts());
			if (myfile_stmt.is_open()) {
				unsigned int num_partitions = context.getNumberPartitions();
				Buffer buffers_stmts[num_partitions];
				Buffer buffers_mirrors_call[num_partitions];
				Buffer buffers_mirrors_shallow[num_partitions];

				string line;
				while (getline(myfile_stmt, line)) {
					if(line == ""){
						continue;
					}

					CFGNode* cfgNode = new CFGNode(line);

					Partition p = context.getPartition(cfgNode->getCfgNodeId());
					//write to the respectively stmt files
					write_to_partition_stmt_binary(p, cfgNode, buffers_stmts[p]);

					//deal with mirrors statements
					if(mirrors_map.find(cfgNode->getCfgNodeId()) != mirrors_map.end()){
						TYPE type = cfgNode->getStmt()->getType();
						if (type == TYPE::Call || type == TYPE::Callfptr || type == TYPE::Calleefptr || type == TYPE::Return) {
							for(auto& partition: mirrors_map[cfgNode->getCfgNodeId()]){
								write_to_partition_mirrors_call_binary(cfgNode, partition, buffers_mirrors_call[partition]);
							}
						}
						else {
							for(auto& partition: mirrors_map[cfgNode->getCfgNodeId()]){
								write_to_partition_mirrors_shallow_binary(cfgNode->getCfgNodeId(), partition, buffers_mirrors_shallow[partition]);
							}
						}
					}

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

					if(!buffers_mirrors_call[i].isEmpty()){
						string mirrors_call_file = Context::folder_mirrors_call + to_string(i);
						FILE *f_mirrors_call = fopen(mirrors_call_file.c_str(), "ab");
						assert(f_mirrors_call);
						fwrite(buffers_mirrors_call[i].getData(), buffers_mirrors_call[i].getSize(), 1, f_mirrors_call);
						buffers_mirrors_call[i].setSize(0);
						fclose(f_mirrors_call);
					}

					if(!buffers_mirrors_shallow[i].isEmpty()){
						string mirrors_shallow_file = Context::folder_mirrors_shallow + to_string(i);
						FILE *f_mirrors_shallow = fopen(mirrors_shallow_file.c_str(), "ab");
						assert(f_mirrors_shallow);
						fwrite(buffers_mirrors_shallow[i].getData(), buffers_mirrors_shallow[i].getSize(), 1, f_mirrors_shallow);
						buffers_mirrors_shallow[i].setSize(0);
						fclose(f_mirrors_shallow);
					}

				}

				myfile_stmt.close();
			}
			cout << "binary stmt done" << endl;


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
//					cout << "id: " << id << endl;
					Partition part = context.getPartition(id);
//					cout << "part: " << part << endl;
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
			cout << "binary actives done" << endl;

		}

		//for debugging
		Logger::print_thread_info_locked("process finished.\n", LEVEL_LOG_FUNCTION);
	}



};



#endif /* PREPROC_PREPROCESS_H_ */
