/*
 * cfg_compute_ooc_syn_cache.h
 *
 *  Created on: Jul 8, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_COMPUTE_OOC_SYN_CACHE_H_
#define COMP_CFG_COMPUTE_OOC_SYN_CACHE_H_


#include "cfg_map_outcore_cache.h"
#include "cfg_compute_syn_cache_naive.h"
#include "cfg_compute_ooc_syn.h"
#include "../preproc/preprocess.h"



using namespace std;

class CFGCompute_ooc_syn_cache {
//
public:

    
    

    static void do_worklist_ooc_synchronous(CFG* cfg_, GraphStore* graphstore, Grammar* grammar, Singletons* singletons, Concurrent_Worklist<CFGNode*>* actives, bool flag, bool update_mode,
			Timer_wrapper_ooc* timer_ooc, Timer_wrapper_inmemory* timer){
                //wait for implementation
    }
    

    static void compute_ooc(CFG_map_outcore* cfg, GraphStore* graphstore, Concurrent_Worklist<CFGNode*>* worklist_1,
			Concurrent_Worklist<CFGNode*>* worklist_2, Grammar* grammar, GraphStore* tmp_graphstore, Singletons* singletons, Concurrent_Worklist<CFGNode*>* actives, bool flag,
			Timer_wrapper_inmemory* timer){
                 //wait for implementation
    }

    static void propagate(CFG_map_outcore* cfg, CFGNode* cfg_node, std::vector<CFGNode*> *pSucs, Concurrent_Worklist<CFGNode *> *pWorklist, Concurrent_Worklist<CFGNode*> *actives) {
        //cout << "successor number: " << pSucs->size() << endl;
        //mtx.lock();
        if (pSucs) {
            for (auto it = pSucs->cbegin(); it != pSucs->cend(); ++it) {
                CFGNode *suc = *it;
                if (!cfg->isMirror(suc)) {
                    pWorklist->push_atomic(suc);
                }
                else{
                    actives->push_atomic(suc);
                }


            }
        }
    }



     static void pass(Partition partition, CFG *cfg, GraphStore *graphstore, Concurrent_Worklist<CFGNode *> *actives,
                     Context *context, bool file_mode) {
        //for debugging
        //cout << "test!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
        Logger::print_thread_info_locked("pass starting...\n", 1);

        //store all the graphs into file
        const string filename_graphs = Context::file_graphstore + to_string(partition);
		//NaiveGraphStore* graphstore_naive = dynamic_cast<NaiveGraphStore*>(graphstore);

        graphstore->serialize(filename_graphs);


        //divide all the activated nodes into multiple partitions
        std::unordered_map<Partition, std::unordered_set<CFGNode *>> map;
        CFGNode *cfg_node;
        while (actives->pop(cfg_node)) {
            Partition partition = context->getPartition(cfg_node);
            if (map.find(partition) != map.end()) {
                map[partition].insert(cfg_node);
            } else {
                map[partition] = std::unordered_set<CFGNode *>();
                map[partition].insert(cfg_node);
            }
        }


        //get the corresponding partition
        for (auto it = map.begin(); it != map.end(); ++it) {
            Partition part = it->first;

            //write actives
            const string file_actives = Context::folder_actives + to_string(part);
            store_actives(file_mode, file_actives, it->second);
            //cout << "active: " << it->second << endl;

            //write graphs_in
            const string file_graphs_in = Context::folder_graphs_in + std::to_string(part);
            // 将active的前置节点的状态也要存下来，以备在包含active节点中的partition更新active节点
            CFGCompute_ooc_syn::store_in_graphs(file_graphs_in, cfg, graphstore, it->second);



            //update the priority set information
            context->update_priority(part, it->second.size());
        }

        //for debugging
        Logger::print_thread_info_locked("pass finished.\n", 1);
    } 








//
//
protected:

    //append the activated nodes into the corresponding file
    static void store_actives(bool file_mode, const string &file_actives, std::unordered_set<CFGNode *> &set) {
        if (file_mode) {
            cout << "wrong file_mode" << endl;
        } else {
            FILE *f_actives = fopen(file_actives.c_str(), "ab");
            if (f_actives == NULL) {
                cout << "can't write to actives_file: " << file_actives << endl;
                exit(-1);
            } else {
                char *buf = (char *) malloc(IO_SIZE);
                long real_io_size = get_real_io_size(IO_SIZE, sizeof(int));
                long offset = 0;
                for (auto &it : set) {
                    int id = it->getCfgNodeId();
                    //cout << "active: " << id << endl;
                    memcpy(buf + offset, (char *) &id, sizeof(int));
                    offset += sizeof(int);

                    if (offset == real_io_size) {
                        fwrite(buf, real_io_size, 1, f_actives);
                        offset = 0;
                    }
                }
                //write the remaining part to file
                fwrite(buf, offset, 1, f_actives);

                free(buf);
                fclose(f_actives);
            }
        }
    }


  

};


#endif /* COMP_CFG_COMPUTE_OOC_SYN_CACHE_H_ */

