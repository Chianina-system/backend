/*
 * cfg_map_outcore.h
 *
 *  Created on: Jul 8, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_MAP_OUTCORE_H_
#define COMP_CFG_MAP_OUTCORE_H_

#include "cfg.h"
#include "../utility/FileUtil.hpp"

class CFG_map_outcore : public CFG {


public:
	CFG_map_outcore(){}

	~CFG_map_outcore(){
//        for (auto &node : nodes) {
//            delete node;
//        }

        for(auto &mirror: mirrors){
//        	assert(mirror->getStmt() == nullptr);
        	delete mirror;
        }
	}

    vector<string> split(const string &s, const string &seperator) {
        vector<string> result;
        typedef string::size_type string_size;
        string_size i = 0;

        while (i != s.size()) {

            int flag = 0;
            while (i != s.size() && flag == 0) {
                flag = 1;
                for (string_size x = 0; x < seperator.size(); ++x)
                    if (s[i] == seperator[x]) {
                        ++i;
                        flag = 0;
                        break;

                    }

            }

            //找到又一个分隔符，将两个分隔符之间的字符串取出；
            flag = 0;
            string_size j = i;
            while (j != s.size() && flag == 0) {
                for (string_size x = 0; x < seperator.size(); ++x)
                    if (s[j] == seperator[x]) {
                        flag = 1;
                        break;

                    }
                if (flag == 0) ++j;
            }
            if (i != j) {
                result.push_back(s.substr(i, j - i));
                i = j;

            }

        }
        return result;

    }

    string filterSpace(string s) {
        int len = s.length();
        int i = 0, j = len - 1;

        while (i < len) {
            if (s[i] == ' '|| s[i] == '\t')
                i++;
            else
                break;
        }

        while (j >=0) {
            if ( s[j]== ' '|| s[j] == '\t')
                j--;
            else
                break;
        }

        if (i == j)return "";
        else  return s.substr(i, j - i + 1);
    }

    CFGNode* getBlock(int ID) {

        for (int i = 0; i < nodes.size(); i++) {
            if (nodes[i]->getCfgNodeId() == ID) {
                return nodes[i];
            }
        }
        return NULL;
    }

    void addNewBlock(CFGNode *pNode) {
        nodes.push_back(pNode);
        //cout << "it's a new block, ID is:" << pNode->getOutPointer() << endl;
    }

//    void print(std::ostream& str) const override {
//    	str << "The number of nodes in CFG: \t" << this->nodes.size() << "\n";
//    	str << "The number of edges in CFG: \t" << this->getNumberEdges() << "\n";
////    	for(auto it = this->succes.begin(); it != this->succes.end(); ++it){
////    		const CFGNode* node = it->first;
////    		for(auto& n: it->second){
////    			str << *node << "\t" << *n << "\n";
////    		}
////    	}
//    }

	inline bool isMirror(CFGNode* node) {
//		return in_mirrors.find(node) != in_mirrors.end() || out_mirrors.find(node) != out_mirrors.end();
		return mirrors.find(node) != mirrors.end();
	}
//
//	inline bool isInMirror(CFGNode* node) {
//		return in_mirrors.find(node) != in_mirrors.end();
//	}
//
//	inline bool isOutMirror(CFGNode* node) {
//		return out_mirrors.find(node) != out_mirrors.end();
//	}

	inline std::unordered_set<CFGNode*>& getActiveNodes(){
		return actives;
	}


	void loadCFG_ooc(bool file_mode, const string& file_cfg, const string& file_stmt, const string& file_mirrors_call, const string& file_mirrors_shallow, const string& file_actives) {
		//for debugging
		Logger::print_thread_info_locked("load-cfg-ooc starting...\n", LEVEL_LOG_FUNCTION);

		std::map<PEGraph_Pointer, CFGNode*> m;

		if(file_mode){
//			// handle the stmt file
//			std::ifstream fin;
//			fin.open(file_stmt);
//			if (!fin) {
//				cout << "can't load file_stmt " << file_stmt << endl;
//				exit(EXIT_FAILURE);
//			}
//			std::string line;
//			while (getline(fin, line)) {
//				if(line == ""){
//					continue;
//				}
//
//				CFGNode* cfgNode = new CFGNode(line);
//				m[cfgNode->getCfgNodeId()] = cfgNode;
//
//				//add cfgnode into cfg
//				this->addOneNode(cfgNode);
//
//	//			//add entry node
//	//			if(cfgNode->getCfgNodeId() == 0){
//	//				this->nodes_entry.push_back(cfgNode);
//	//			}
//			}
//			fin.close();
//
//
//			//handle mirrors file
//			fin.open(file_mirrors_call);
//			if (!fin) {
//				cout << "can't load file_mirrors_call: " << file_mirrors_call << endl;
//			}
//			else {
//				while (getline(fin, line)) {
//					if(line == ""){
//						continue;
//					}
//
//					CFGNode* cfgNode_mirror = new CFGNode(line);
//					assert(m.find(cfgNode_mirror->getCfgNodeId()) == m.end());
//					m[cfgNode_mirror->getCfgNodeId()] = cfgNode_mirror;
//
////					PEGraph_Pointer id = atoi(line.c_str());
////					if(m.find(id) == m.end()){
////						m[id] = new CFGNode(id, nullptr);
////					}
//					this->mirrors.insert(cfgNode_mirror);
//				}
//				fin.close();
//			}
//
//			fin.open(file_mirrors_shallow);
//			if (!fin) {
//				cout << "can't load file_mirrors_shallow: " << file_mirrors_shallow << endl;
//			}
//			else{
//				while (getline(fin, line)) {
//					if(line == ""){
//						continue;
//					}
//
//					PEGraph_Pointer id = atoi(line.c_str());
//					if(m.find(id) == m.end()){
//						m[id] = new CFGNode(id, nullptr);
//					}
//					this->mirrors.insert(m[id]);
//				}
//				fin.close();
//			}
//
//
//			//handle the cfg file
//			fin.open(file_cfg);
//			if (!fin) {
//				cout << "can't load file_cfg: " << file_cfg << endl;
//				exit(EXIT_FAILURE);
//			}
//			while (getline(fin, line)) {
//				if(line == ""){
//					continue;
//				}
//
//				std::stringstream stream(line);
//				std::string pred_id, succ_id;
//				stream >> pred_id >> succ_id;
//
//				//        cfgMap->addOneNode(m[atoi(pred_id.c_str())]);
//				//        cfgMap->addOneNode(m[atoi(succ_id.c_str())]);
//				this->addOneSucc(m[atoi(pred_id.c_str())],
//						m[atoi(succ_id.c_str())]);
//				this->addOnePred(m[atoi(succ_id.c_str())],
//						m[atoi(pred_id.c_str())]);
//			}
//			fin.close();
//
//
//
//			//handle actives file
//			fin.open(file_actives);
//			if (!fin) {
//				cout << "can't load file_actives: " << file_actives << endl;
////				exit(EXIT_FAILURE);
//			}
//			while (getline(fin, line)) {
//				if(line == ""){
//					continue;
//				}
//
//	//			std::stringstream stream(line);
//	//			std::string item;
//	//			while(getline(stream, item, '\t')){
//	//				this->actives.insert(m[atoi(item.c_str())]);
//	//			}
//				this->actives.insert(m[atoi(line.c_str())]);
//			}
//			fin.close();
//
//			//delete the actives file
//			FileUtil::delete_file(file_actives);
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

					CFGNode* cfgNode = new CFGNode();
					cfgNode->read_from_buf(buf, bufsize);
					if (2463342 == cfgNode->getCfgNodeId()) cout << "have 2463342" << endl;
					m[cfgNode->getCfgNodeId()] = cfgNode;
//					cout << "cfg node: " << cfgNode->getCfgNodeId() << endl;
//                    cout << "content: ";
//                    for(int i = 0; i < cfgNode->getAllContent().size(); i++) {
//					    cout << cfgNode->getAllContent()[i] << ' ';
//					}
//                    cout << endl;

					//add cfgnode into cfg
					this->addOneNode(cfgNode);

					free(buf);
				}
				fclose(fp);
    		}


			//handle mirrors file
//    		FILE* fp_mirrors_call = fopen(file_mirrors_call.c_str(), "rb");
//    		if(!fp_mirrors_call) {
//    			cout << "can't load file_mirrors_call: " << file_mirrors_call << endl;
////    			exit(-1);
//    		}
//    		else{
//				size_t freadRes = 0; //clear warnings
//				size_t bufsize;
//				while(fread(&bufsize, sizeof(size_t), 1, fp_mirrors_call) != 0) {
//					char *buf = (char*)malloc(bufsize);
//					freadRes = fread(buf, bufsize, 1, fp_mirrors_call);
//
//					CFGNode* cfgNode = new CFGNode();
//					cfgNode->read_from_buf(buf, bufsize);
//					assert(m.find(cfgNode->getCfgNodeId()) == m.end());
//					m[cfgNode->getCfgNodeId()] = cfgNode;
//					this->mirrors.insert(cfgNode);
//
//					free(buf);
//				}
//				fclose(fp_mirrors_call);
//    		}

			int fp_mirrors_shallow = open(file_mirrors_shallow.c_str(), O_RDONLY);
    		if(!(fp_mirrors_shallow > 0)) {
    			cout << "can't load file_mirrors_shallow: " << file_mirrors_shallow << endl;
//    			exit(-1);
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
							m[id] = new CFGNode(id, nullptr);
						}
						this->mirrors.insert(m[id]);
						//cout << "id: " << m[id]->getCfgNodeId() << endl;
					}
				}
				free(buf);

				close(fp_mirrors_shallow);
    		}


			//handle the cfg file
			int fp_cfg = open(file_cfg.c_str(), O_RDONLY);
    		if(!(fp_cfg > 0)) {
    			cout << "can't load file_cfg: " << file_cfg << endl;
//    			exit(-1);
    		}
    		else{
				long cfg_file_size = io_manager::get_filesize(fp_cfg);
				char* buf = (char*)memalign(PAGE_SIZE, IO_SIZE);
				long real_io_size = get_real_io_size(IO_SIZE, sizeof(vertexid_t) * 2 + sizeof(char));
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

					for(long offset = 0; offset < valid_io_size; offset += (sizeof(vertexid_t) * 2 + sizeof(char))) {
						vertexid_t pred_id = *((vertexid_t*)(buf + offset));
						vertexid_t succ_id = *((vertexid_t*)(buf + offset + sizeof(vertexid_t)));
						char type = *((char*)(buf + offset + sizeof(vertexid_t) + sizeof(vertexid_t)));
						if (pred_id == 2451646) {
						    cout << "pred_id: " << pred_id;
						    cout << " succ_id: " << succ_id;
						    cout << " type:" << type << endl;
						}

                        if (type == 'T' ) {
                            //src->addTchild(dst->getCfgNodeId());
                            this->addOneSucc(m[pred_id], m[succ_id],  "TEdge", 0);
                            this->addOnePred(m[succ_id], m[pred_id]);
                        }
                        else if (type == 'C') {
                            this->addOneSucc(m[pred_id], m[succ_id],  "CallEdge", 0);
                            this->addOnePred(m[succ_id], m[pred_id]);
                        }
                        else if(type == 'B' || type == 'R') {
                            //this->addBackEdge(stoi(edgePair[0]), stoi(edgePair[1]));
                            this->addOneSucc(m[pred_id], m[succ_id], "BackEdge", 0);
                            this->addOnePred(m[succ_id], m[pred_id]);
                        }
                        else {
                            //src->addFchild(dst->getCfgNodeId());
                            this->addOneSucc(m[pred_id], m[succ_id], "FEdge", 0);
                            this->addOnePred(m[succ_id], m[pred_id]);
                        }

//						this->addOneSucc(m[pred_id], m[succ_id]);
//						this->addOnePred(m[succ_id], m[pred_id]);
//						cout << "m[pred_id], m[succ_id]: " << m[pred_id]->getCfgNodeId() << " " << m[succ_id]->getCfgNodeId()
//						<< " type: " << type << endl;
					}
				}
				free(buf);

				close(fp_cfg);
    		}


			//handle actives file
			int fp_actives = open(file_actives.c_str(), O_RDONLY);
    		if(!(fp_actives > 0)) {
    			cout << "can't load file_actives: " << file_actives << endl;
//    			exit(-1);
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
						//cout << "active: " << m[id]->getCfgNodeId() << endl;
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


//    std::vector<CFGNode*>* getPredesessors(const CFGNode* node) {
//        auto it = predes.find(node);
//        if(it != predes.end()){
//            return &(it->second);
//        }
//        else{
//        	return std::vector<CFGNode*>();
////            perror("invalid key!");
////            exit (EXIT_FAILURE);
//        }
//    }
//
//    std::vector<CFGNode*>* getSuccessors(const CFGNode* node) {
//        auto it = succes.find(node);
//        if(it != succes.end()){
//            return &(it->second);
//        }
//        else{
//        	return std::vector<CFGNode*>();
////            perror("invalid key!");
////            exit (EXIT_FAILURE);
//        }
//    }


    void addOneNode(CFGNode *Node)  {
        nodes.push_back(Node);
    }


    void addOnePred(CFGNode *succ, CFGNode *pred)  {
		if(predes.find(succ) == predes.end()) {
			predes[succ] = std::vector<CFGNode*>();
		}
		predes[succ].push_back(pred);
    }

    void addOneSucc(CFGNode *pred, CFGNode *succ, string edgeType, int num)  {
        if(succes.find(pred) == succes.end()){
            succes[pred] = std::vector<suc*>();
            //succes[pred] = std::vector<std::pair<CFGNode*, std::string>>();
            //succes[pred] = tmp;
        }
        //succes[pred].push_back(succ);
        suc * tmp = new suc(succ, edgeType, num);
        //std::pair<CFGNode*, std::string> p1(succ,edgeType);
        succes[pred].push_back(tmp);
        //this->number_edges++;
    }

//    void addOneSucc(CFGNode *pred, CFGNode *succ)  {
//    	if(succes.find(pred) == succes.end()){
//    		succes[pred] = std::vector<CFGNode*>();
//    	}
//		succes[pred].push_back(succ);
//		this->number_edges++;
//    }
    void print(std::ostream& str) const override {
        str << "The number of nodes in CFG: \t" << this->nodes.size() << "\n";
//        str << "The number of edges in CFG: \t" << this->getNumberEdges() << "\n";
        for (auto it = this->succes.begin(); it != this->succes.end(); ++it) {
            const CFGNode* node = it->first;
            for(auto& n: it->second){
                // str << *node << "\t" << *(n.first) << "\t" << n.second << "\n";
            }
        }
    }

    void print() {
	    cout << "The number of nodes in CFG: \t" << this->nodes.size() << "\n";
        for (auto it = this->succes.begin(); it != this->succes.end(); ++it) {
            cout << "it->second.size(): " <<  it->second.size() << endl;
            for (int i = 0; i < it->second.size(); i ++) {
                cout << "test1" << endl;
                cout << it->first->getCfgNodeId();
                cout << "test2" << endl;
                cout << " - ";
                if (it->second[i] == nullptr) cout << "null!" << endl;
                if (it->second[i]->node == nullptr) cout << "node null!" << endl;
                cout << it->second[i]->node->getCfgNodeId();
                cout << "test3" << endl;
                cout << " " ;
                cout << it->second[i]->type << endl;
                cout << "test4" << endl;
            }
        }
	}




private:
//	std::unordered_set<CFGNode*> in_mirrors;
//
//	std::unordered_set<CFGNode*> out_mirrors;

	std::unordered_set<CFGNode*> mirrors;

	std::unordered_set<CFGNode*> actives;



};



#endif /* COMP_CFG_MAP_OUTCORE_H_ */
