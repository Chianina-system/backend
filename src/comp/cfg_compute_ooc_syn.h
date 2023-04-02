/*
 * cfg_compute_ooc.h
 *
 *  Created on: Jul 8, 2019
 *      Author: zqzuo
 */

#ifndef COMP_CFG_COMPUTE_OOC_SYN_H_
#define COMP_CFG_COMPUTE_OOC_SYN_H_


#include "cfg_map_outcore.h"
#include "../preproc/preprocess.h"
#include "cfg_compute_syn_itemset.h"
#include "cfg_compute_syn_naive.h"
#include "cfg_compute_syn_delta.h"


using namespace std;

class CFGCompute_ooc_syn {
//
public:

    static bool
    load(Partition part, CFG *cfg_, GraphStore *graphstore, Context *context, bool file_mode, int mining_mode,
         int support, int length) {
        //for debugging
        Logger::print_thread_info_locked("load starting...\n", LEVEL_LOG_FUNCTION);

        string partition = to_string(part);
        const string filename_cfg = Context::file_cfg + partition;
        const string filename_mirrors_in = Context::folder_mirrors_call + partition;
        const string filename_mirrors_out = Context::folder_mirrors_shallow + partition;
        const string foldername_actives = Context::folder_actives + partition;
        const string filename_stmt = Context::file_stmts + partition;
        const string filename_singleton = context->getFileSingletons();
        const string filename_graphs = Context::file_graphstore + partition;
        const string foldername_graphs_in = Context::folder_graphs_in + partition;

        CFG_map_outcore *cfg = dynamic_cast<CFG_map_outcore *>(cfg_);
        cfg->loadCFG_ooc(file_mode, filename_cfg, filename_stmt, filename_mirrors_in, filename_mirrors_out,
                         foldername_actives);
        //cfg->print();
        //cout << "test!" << endl;

//		graphstore->init(cfg_);
        graphstore->loadGraphStore(filename_graphs, foldername_graphs_in, part, mining_mode, support, length);

        //for debugging
        Logger::print_thread_info_locked("load finished.\n", LEVEL_LOG_FUNCTION);

        return true;
    }

//
//
    static void pass(Partition partition, CFG *cfg, GraphStore *graphstore, Concurrent_Worklist<CFGNode *> *actives,
                     Context *context, bool file_mode) {
        //for debugging
        //cout << "test!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
        Logger::print_thread_info_locked("pass starting...\n", 1);

        //store all the graphs into file
        const string filename_graphs = Context::file_graphstore + to_string(partition);
//		NaiveGraphStore* graphstore_naive = dynamic_cast<NaiveGraphStore*>(graphstore);

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
            store_in_graphs(file_graphs_in, cfg, graphstore, it->second);
            
            context->update_priority(part, it->second.size());
        }

        //for debugging
        Logger::print_thread_info_locked("pass finished.\n", 1);
    }

private:

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


    //append the updated in_mirrors into the corresponding file
    static void store_in_graphs(const string &file_graphs_in, CFG *cfg, GraphStore *graphstore,
                                std::unordered_set<CFGNode *> &set) {
        //get all the predecessors
        std::unordered_set<CFGNode *> s;
        for (auto &it: set) {
            CFGNode *node_dst = it;
            auto nodes = cfg->getPredesessors(node_dst);
            if (nodes) {
                for (auto &n: *nodes) {
                    s.insert(n);
                }
            }
        }

		//write the corresponding state into file
		graphstore->store_in_graphs(file_graphs_in, s);
    };

};


#endif /* COMP_CFG_COMPUTE_OOC_SYN_H_ */
