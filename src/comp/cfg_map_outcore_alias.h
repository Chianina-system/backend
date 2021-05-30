/*
 * cfg_map_outcore_alias.h
 *
 *  Created on: Jul 8, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_MAP_OUTCORE_ALIAS_H_
#define COMP_CFG_MAP_OUTCORE_ALIAS_H_

#include "cfg_map_outcore.h"
#include "../utility/FileUtil.hpp"
#include "cfg_node_alias.h"

using namespace std;

class CFG_map_outcore_alias : public  CFG_map_outcore {


public:
	CFG_map_outcore_alias(){}

	~CFG_map_outcore_alias(){
        for(auto &mirror: mirrors){
        	//assert(mirror->getStmt() == nullptr);
        	delete mirror;
        }
	}

    void print(std::ostream& str) const override {
    	str << "The number of nodes in CFG: \t" << this->nodes.size() << "\n";
    	str << "The number of edges in CFG: \t" << this->getNumberEdges() << "\n";
    	for(auto it = this->succes.begin(); it != this->succes.end(); ++it){
    		// CFGNode_alias* node = dynamic_cast<CFGNode_alias>(it->first);
    		for(auto& n: it->second){
    			//str << *node << "\t" << *n << "\n";
    		}
    	}
    }

	

void loadCFG_ooc(bool file_mode, const string& file_cfg, const string& file_stmt, const string& file_mirrors_call, const string& file_mirrors_shallow, const string& file_actives) {
		//for debugging
		Logger::print_thread_info_locked("load-cfg-ooc starting...\n", LEVEL_LOG_FUNCTION);

		std::map<PEGraph_Pointer, CFGNode*> m;

		if(file_mode){
			// handle the stmt file
			std::ifstream fin;
			fin.open(file_stmt);
			if (!fin) {
				cout << "can't load file_stmt " << file_stmt << endl;
				exit(EXIT_FAILURE);
			}
			std::string line;
			while (getline(fin, line)) {
				if(line == ""){
					continue;
				}

				CFGNode* cfgNode = new CFGNode_alias(line);
				m[cfgNode->getCfgNodeId()] = cfgNode;

				//add cfgnode into cfg
				this->addOneNode(cfgNode);

	//			//add entry node
	//			if(cfgNode->getCfgNodeId() == 0){
	//				this->nodes_entry.push_back(cfgNode);
	//			}
			}
			fin.close();


			//handle mirrors file
			fin.open(file_mirrors_call);
			if (!fin) {
				cout << "can't load file_mirrors_call: " << file_mirrors_call << endl;
			}
			else {
				while (getline(fin, line)) {
					if(line == ""){
						continue;
					}

					CFGNode* cfgNode_mirror = new CFGNode_alias(line);
					assert(m.find(cfgNode_mirror->getCfgNodeId()) == m.end());
					m[cfgNode_mirror->getCfgNodeId()] = cfgNode_mirror;

//					PEGraph_Pointer id = atoi(line.c_str());
//					if(m.find(id) == m.end()){
//						m[id] = new CFGNode(id, nullptr);
//					}
					this->mirrors.insert(cfgNode_mirror);
				}
				fin.close();
			}

			fin.open(file_mirrors_shallow);
			if (!fin) {
				cout << "can't load file_mirrors_shallow: " << file_mirrors_shallow << endl;
			}
			else{
				while (getline(fin, line)) {
					if(line == ""){
						continue;
					}

					PEGraph_Pointer id = atoi(line.c_str());
					if(m.find(id) == m.end()){
						m[id] = new CFGNode_alias(id, nullptr);
					}
					this->mirrors.insert(m[id]);
				}
				fin.close();
			}


			//handle the cfg file
			fin.open(file_cfg);
			if (!fin) {
				cout << "can't load file_cfg: " << file_cfg << endl;
				exit(EXIT_FAILURE);
			}
			while (getline(fin, line)) {
				if(line == ""){
					continue;
				}

				std::stringstream stream(line);
				std::string pred_id, succ_id;
				stream >> pred_id >> succ_id;

				//        cfgMap->addOneNode(m[atoi(pred_id.c_str())]);
				//        cfgMap->addOneNode(m[atoi(succ_id.c_str())]);
				this->addOneSucc(m[atoi(pred_id.c_str())],
						m[atoi(succ_id.c_str())]);
				this->addOnePred(m[atoi(succ_id.c_str())],
						m[atoi(pred_id.c_str())]);
			}
			fin.close();



			//handle actives file
			fin.open(file_actives);
			if (!fin) {
				cout << "can't load file_actives: " << file_actives << endl;
//				exit(EXIT_FAILURE);
			}
			while (getline(fin, line)) {
				if(line == ""){
					continue;
				}

	//			std::stringstream stream(line);
	//			std::string item;
	//			while(getline(stream, item, '\t')){
	//				this->actives.insert(m[atoi(item.c_str())]);
	//			}
				this->actives.insert(m[atoi(line.c_str())]);
			}
			fin.close();

			//delete the actives file
			FileUtil::delete_file(file_actives);
		}
		else{//binary format
			// handle the stmt file
    		FILE *fp = fopen(file_stmt.c_str(),"rb");
    		if(!fp) {
    			cout << "can't load file_stmt: " << file_stmt << endl;
//    			exit(-1);
    		}
    		else{
				size_t freadRes = 0; //clear warnings
				size_t bufsize;
				while(fread(&bufsize, sizeof(size_t), 1, fp) != 0) {
					char *buf = (char*)malloc(bufsize);
					freadRes = fread(buf, bufsize, 1, fp);

					CFGNode* cfgNode = new CFGNode_alias();
					cfgNode->read_from_buf(buf, bufsize);
					m[cfgNode->getCfgNodeId()] = cfgNode;

					//add cfgnode into cfg
					this->addOneNode(cfgNode);

					free(buf);
				}
				fclose(fp);
    		}


			//handle mirrors file
    		FILE* fp_mirrors_call = fopen(file_mirrors_call.c_str(), "rb");
    		if(!fp_mirrors_call) {
    			cout << "can't load file_mirrors_call: " << file_mirrors_call << endl;
//    			exit(-1);
    		}
    		else{
				size_t freadRes = 0; //clear warnings
				size_t bufsize;
				while(fread(&bufsize, sizeof(size_t), 1, fp_mirrors_call) != 0) {
					char *buf = (char*)malloc(bufsize);
					freadRes = fread(buf, bufsize, 1, fp_mirrors_call);

					CFGNode* cfgNode = new CFGNode_alias();
					cfgNode->read_from_buf(buf, bufsize);
					assert(m.find(cfgNode->getCfgNodeId()) == m.end());
					m[cfgNode->getCfgNodeId()] = cfgNode;
					this->mirrors.insert(cfgNode);

					free(buf);
				}
				fclose(fp_mirrors_call);
    		}

			int fp_mirrors_shallow = open(file_mirrors_shallow.c_str(), O_RDONLY);
    		if(!(fp_mirrors_shallow > 0)) {
    			cout << "can't load file_mirrors_shallow: " << file_mirrors_shallow << endl;
    		}
    		else{
				long mirrors_out_file_size = io_manager::get_filesize(fp_mirrors_shallow);
				char* buf = (char*)memalign(PAGE_SIZE, IO_SIZE);
				long real_io_size = get_real_io_size(IO_SIZE, sizeof(vertexid_t));
				int streaming_counter = mirrors_out_file_size / real_io_size + 1;

				long offset_read = 0;

				for(int counter = 0; counter < streaming_counter; counter++) {
					long valid_io_size = 0;
					if (counter == streaming_counter - 1)
						valid_io_size = mirrors_out_file_size - real_io_size * (streaming_counter - 1);
					else
						valid_io_size = real_io_size;

					io_manager::read_from_file(fp_mirrors_shallow, buf, valid_io_size, offset_read);
					offset_read += valid_io_size;

					for(long offset = 0; offset < valid_io_size; offset += sizeof(vertexid_t)) {
						PEGraph_Pointer id = *((vertexid_t*)(buf + offset));
						if (m.find(id) == m.end()) {
							m[id] = new CFGNode_alias(id, nullptr);
						}
						this->mirrors.insert(m[id]);
					}
				}
				free(buf);

				close(fp_mirrors_shallow);
    		}


			//handle the cfg file
			int fp_cfg = open(file_cfg.c_str(), O_RDONLY);
    		if(!(fp_cfg > 0)) {
    			cout << "can't load file_cfg: " << file_cfg << endl;
    		}
    		else{
				long cfg_file_size = io_manager::get_filesize(fp_cfg);
				char* buf = (char*)memalign(PAGE_SIZE, IO_SIZE);
				long real_io_size = get_real_io_size(IO_SIZE, sizeof(vertexid_t) * 2);
				int streaming_counter = cfg_file_size / real_io_size + 1;

				long offset_read = 0;

				for(int counter = 0; counter < streaming_counter; counter++) {
					long valid_io_size = 0;
					if (counter == streaming_counter - 1)
						valid_io_size = cfg_file_size - real_io_size * (streaming_counter - 1);
					else
						valid_io_size = real_io_size;

					io_manager::read_from_file(fp_cfg, buf, valid_io_size, offset_read);
					offset_read += valid_io_size;

					for(long offset = 0; offset < valid_io_size; offset += sizeof(vertexid_t) * 2) {
						vertexid_t pred_id = *((vertexid_t*)(buf + offset));
						vertexid_t succ_id = *((vertexid_t*)(buf + offset + sizeof(vertexid_t)));

						this->addOneSucc(m[pred_id], m[succ_id]);
						this->addOnePred(m[succ_id], m[pred_id]);
					}
				}
				free(buf);

				close(fp_cfg);
    		}


			//handle actives file
			int fp_actives = open(file_actives.c_str(), O_RDONLY);
    		if(!(fp_actives > 0)) {
    			cout << "can't load file_actives: " << file_actives << endl;
    		}
    		else{
				long actives_file_size = io_manager::get_filesize(fp_actives);
				char* buf = (char*)memalign(PAGE_SIZE, IO_SIZE);
				long real_io_size = get_real_io_size(IO_SIZE, sizeof(vertexid_t));
				int streaming_counter = actives_file_size / real_io_size + 1;

				long offset_read = 0;

				for(int counter = 0; counter < streaming_counter; counter++) {
					long valid_io_size = 0;
					if (counter == streaming_counter - 1)
						valid_io_size = actives_file_size - real_io_size * (streaming_counter - 1);
					else
						valid_io_size = real_io_size;

					io_manager::read_from_file(fp_actives, buf, valid_io_size, offset_read);
					offset_read += valid_io_size;

					for(long offset = 0; offset < valid_io_size; offset += sizeof(vertexid_t)) {
						PEGraph_Pointer id = *((vertexid_t*)(buf + offset));
						this->actives.insert(m[id]);
					}
				}
				free(buf);

				close(fp_actives);

				//delete the actives file
				FileUtil::delete_file(file_actives);
    		}

		}


		//for debugging
		Logger::print_thread_info_locked("load-cfg-ooc finished.\n", LEVEL_LOG_FUNCTION);
	}

   


private:
//	std::unordered_set<CFGNode*> in_mirrors;
//
//	std::unordered_set<CFGNode*> out_mirrors;

	//std::unordered_set<CFGNode*> mirrors;

	//std::unordered_set<CFGNode*> actives;



};



#endif /* COMP_CFG_MAP_OUTCORE_ALIAS_H_ */


