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

public:


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


	static void process(Context& context){
		//for debugging
		Logger::print_thread_info_locked("process starting...\n", LEVEL_LOG_FUNCTION);

		//clear the folder for storing the tmp data
		FileUtil::reset_folder(Context::working_path);

		//deal with cfg file
		ifstream myfile_cfg(context.getFileCfg());
		if (myfile_cfg.is_open()) {
			string line;
			while (getline(myfile_cfg, line) && line != "") {
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
			while (getline(myfile_stmt, line) && line != "") {
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

		//tag entry partitions according to entry nodes
		context.update_priority(context.getPartition(0), 1);

		//initialize active nodes
		write_to_partition_actives(context.getPartition(0), 0);


		//for debugging
		Logger::print_thread_info_locked("process finished.\n", LEVEL_LOG_FUNCTION);
	}



};



#endif /* PREPROC_PREPROCESS_H_ */
