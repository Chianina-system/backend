/*
 * cfg_map_outcore.h
 *
 *  Created on: Jul 8, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_MAP_OUTCORE_CACHE_H_
#define COMP_CFG_MAP_OUTCORE_CACHE_H_

#include "cfg_map_outcore.h"
#include "cfg_node_cache.h"
#include "../utility/FileUtil.hpp"

using namespace std;

class CFG_map_outcore_cache : public CFG_map_outcore {


public:
	CFG_map_outcore_cache(){}

	~CFG_map_outcore_cache(){
        for(auto &mirror: mirrors){
        	//assert(mirror->getStmt() == nullptr);
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



   void loadCFG_ooc(bool file_mode, const string& file_cfg, const string& file_stmt, const string& file_mirrors_call, const string& file_mirrors_shallow, const string& file_actives){

		//for debugging
		Logger::print_thread_info_locked("load-cfg-ooc starting...\n", LEVEL_LOG_FUNCTION);

		std::map<PEGraph_Pointer, CFGNode*> m;

		if(file_mode){
			// handle the stmt file
		}
		else{//binary format
			// handle the stmt file
    		FILE *fp = fopen(file_stmt.c_str(),"rb");
    		if(!fp) {
    			cout << "can't load file_stmt: " << file_stmt << endl;
    			//exit(-1);
    		}
    		else{
				size_t freadRes = 0; //clear warnings
				size_t bufsize;
				while(fread(&bufsize, sizeof(size_t), 1, fp) != 0) {
					char *buf = (char*)malloc(bufsize);
					freadRes = fread(buf, bufsize, 1, fp);

					CFGNode* cfgNode = new CFGNode_cache();
					cfgNode->read_from_buf(buf, bufsize);
					//if (2463342 == cfgNode->getCfgNodeId()) cout << "have 2463342" << endl;
					m[cfgNode->getCfgNodeId()] = cfgNode;

					//add cfgnode into cfg
					this->addOneNode(cfgNode);

					free(buf);
				}
				fclose(fp);
    		}


			//handle mirrors file
			int fp_mirrors_shallow = open(file_mirrors_shallow.c_str(), O_RDONLY);
    		if(!(fp_mirrors_shallow > 0)) {
    			cout << "can't load file_mirrors_shallow: " << file_mirrors_shallow << endl;
    			//exit(-1);
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
							m[id] = new CFGNode_cache(id);
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
    			//exit(-1);
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

					for(long offset = 0; offset < valid_io_size; offset += (sizeof(vertexid_t) * 2)) {
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
    			//exit(-1);
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

	//inline bool isMirror(CFGNode* node) {
		//return in_mirrors.find(node) != in_mirrors.end() || out_mirrors.find(node) != out_mirrors.end();
	//	return mirrors.find(node) != mirrors.end();
	//}


	//inline std::unordered_set<CFGNode*>& getActiveNodes(){
	//	return actives;
	//}

    void print(std::ostream& str) const override {
        str << "The number of nodes in CFG: \t" << this->nodes.size() << "\n";
        //str << "The number of edges in CFG: \t" << this->getNumberEdges() << "\n";
        for (auto it = this->succes.begin(); it != this->succes.end(); ++it) {
            // CFGNode_cache* node = it->first;
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
                if (it->second[i] == nullptr) cout << "node null!" << endl;
                cout << it->second[i]->getCfgNodeId();
                cout << "test3" << endl;
                cout << " " ;
                cout << "test4" << endl;
            }
        }
	}




private:
//	std::unordered_set<CFGNode*> in_mirrors;
//
//	std::unordered_set<CFGNode*> out_mirrors;

	

};



#endif /* COMP_CFG_MAP_OUTCORE_CACHE_H_ */

