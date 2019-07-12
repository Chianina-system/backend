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

	static void process(Context& context){
		//deal with cfg file
		ifstream myfile(Context::file_cfg);
		if (myfile.is_open()) {
			string line;
			while (getline(myfile, line) && line != "") {
				std::stringstream stream(line);
				std::string pred_id, succ_id;
				stream >> pred_id >> succ_id;
				Partition src_p = context.getPartition(atoi(pred_id.c_str()));
				Partition dst_p = context.getPartition(atoi(succ_id.c_str()));
				//write to the respectively cfg files
				if(src_p == dst_p){
					write_to_partition_cfg(src_p, line);
				}
				else{
					write_to_partition_cfg(src_p, line);
					write_to_partition_cfg(dst_p, line);
				}
			}
			myfile.close();
		}

		//deal with stmt file
		ifstream myfile_stmt(Context::file_stmts);
		if (myfile_stmt.is_open()) {
			string line;
			while (getline(myfile_stmt, line) && line != "") {
				std::stringstream stream(line);
				std::string id;
				stream >> id;
				Partition p = context.getPartition(atoi(id.c_str()));
				//write to the respectively stmt files
				write_to_partition_stmt(p, line);
			}
			myfile_stmt.close();
		}


	}



};



#endif /* PREPROC_PREPROCESS_H_ */
