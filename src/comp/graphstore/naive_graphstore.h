/*
 * NaiveGraphStore.h
 *
 *  Created on: Jun 27, 2019
 *      Author: zqzuo
 */

#ifndef COMP_GRAPHSTORE_NAIVE_GRAPHSTORE_H_
#define COMP_GRAPHSTORE_NAIVE_GRAPHSTORE_H_

#include "../../utility/Logger.hpp"
#include "../../utility/FileUtil.hpp"
#include "graphstore.h"
#include "../../myTimer.h"
//#include "cfg_compute.h"

using namespace std;

class NaiveGraphStore : public GraphStore {

public:
    NaiveGraphStore() {}


    ~NaiveGraphStore() {
//    	cout << "deleting naive-graph-store..." << endl;
        for (auto it = map.begin(); it != map.end(); ++it) {
//    		cout << "deleting pegraph " << it->first << endl;
            delete it->second;
        }
    }

    void init(CFG *cfg) {
        std::vector<CFGNode *> nodes = cfg->getNodes();
        for (auto &node: nodes) {
            PEGraph_Pointer graph_pointer = node->getOutPointer();
            if (map.find(graph_pointer) == map.end()) {
                map[graph_pointer] = new PEGraph();
            }
        }
    }


    void load_onebyone(const string &file) {
        //for debugging
        Logger::print_thread_info_locked("load-readable starting...\n", LEVEL_LOG_FUNCTION);

        std::ifstream fin;
        fin.open(file);
        if (!fin) {
            cout << "can't load file_graphs: " << file << endl;
//	        exit (EXIT_FAILURE);
        } else {
            std::string line;
            while (getline(fin, line)) {
                if (line == "") {
                    continue;
                }

                std::stringstream stream(line);
                std::string id;
                stream >> id;
                PEGraph_Pointer graph_pointer = atoi(id.c_str());
                PEGraph *pegraph = new PEGraph();
                pegraph->load_readable(stream);
                //since the file is appended, we just use the recent updated pegraph
                if (map.find(graph_pointer) != map.end()) {
                    delete map[graph_pointer];
                }
                map[graph_pointer] = pegraph;
            }
            fin.close();

            //delete the old graphstore file
            FileUtil::delete_file(file);
        }

        //for debugging
        Logger::print_thread_info_locked("load-readable finished.\n", LEVEL_LOG_FUNCTION);

    }


    void loadGraphStore(const string &file, const string &folder_in) {
        //graphstore file
        this->deserialize(file);

        //updated graphstore
//        DIR* dirp = opendir(folder_in.c_str());
//        if(dirp){
//			struct dirent * dp;
//			while ((dp = readdir(dirp)) != NULL) {
//				if(strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
//					cout << "file name: " << dp->d_name << endl;
//					this->load_onebyone(folder_in + "/" + dp->d_name);
//				}
//			}
//			closedir(dirp);
//        }
//        else{
//        	cout << "can't load folder: " << folder_in << endl;
//        }

        this->load_onebyone(folder_in);
    }


//    void doSerialize(ostringstream &out) {
//        int size1 = map.bucket_count();
//        out.write(reinterpret_cast<char *>(&size1), sizeof(size1));
//
//        for (auto pointerToPEGraph: map) {
//            int first = pointerToPEGraph.first;
//            out.write(reinterpret_cast<char *>(&first), sizeof(first));
//
//            unordered_map<vertexid_t, EdgeArray> &map2 = pointerToPEGraph.second->getGraph();
//            auto size2 = map2.bucket_count();
//            out.write(reinterpret_cast<char *>(&size2), sizeof(size2));
//
//            for (auto &vertexidToEdgeArray: map2) {
//                vertexid_t first1 = vertexidToEdgeArray.first;
//                out.write(reinterpret_cast<char *>(&first1),
//                          sizeof(first1));       //todo 不要忘记加&
//                int capacity = vertexidToEdgeArray.second.getCapacity();
//                int size = vertexidToEdgeArray.second.getSize();
//                vertexid_t *edges = vertexidToEdgeArray.second.getEdges();
//                label_t *labels = vertexidToEdgeArray.second.getLabels();
//                out.write(reinterpret_cast<char *>(&capacity), sizeof(capacity));
//                out.write(reinterpret_cast<char *>(&size), sizeof(size));
//                for (int i = 0; i < size; i++) {
//                    out.write(reinterpret_cast<char *>(&edges[i]), sizeof(edges[i]));
//                    out.write(reinterpret_cast<char *>(&labels[i]), sizeof(labels[i]));
//                }
//            }
//        }
//    }

//    void startSerialize(const string &file, ostringstream &out) {
//        doSerialize(out);
//    }

/* unreadable(使用二进制)序列化参考:
 * http://zxi.mytechroad.com/blog/tree/leetcode-297-serialize-and-deserialize-binary-tree/
 * */
    void serialize(const string &file) {
        ofstream myfile;
        myfile.open(file, std::ofstream::out);
        if (myfile.is_open()) {
            if (readable) {
                for (auto &n : map) {
                    //write a pegraph into file
                    myfile << n.first << "\t";
                    n.second->write_readable(myfile);
                    myfile << "\n";
                }
            } else {
                ostringstream out;
                int size = map.bucket_count();
                out.write(reinterpret_cast<char *>(&size), sizeof(size));
                for (auto &n : map) {
                    PEGraph_Pointer pointer = n.first;
                    out.write(reinterpret_cast<char *>(&pointer), sizeof(pointer));
                    n.second->write_unreadable(out);
                }
            // todo 最后一起执行 myfile << out.str()
                myfile << out.str();
            }
            myfile.close();
        } else {
            cout << "open file:" << file << "failed" << endl;
        }
    }

/* unreadable(使用二进制)序列化参考:
 * http://zxi.mytechroad.com/blog/tree/leetcode-297-serialize-and-deserialize-binary-tree/
 * */
    void deserialize(const string &file) {
        if (readable) {
            load_onebyone(file);
        } else {
            load_unreadable(file);
        }
    }

    void load_unreadable(const string &file) const {
        ifstream fin;
        fin.open(file);
        if (!fin) {
            cout << "can't load file_graphs: " << file << endl;
        } else {
            //一次性读取全部
            string str((istreambuf_iterator<char>(fin)), istreambuf_iterator<char>());
            stringstream in(str);

            PEGraph_Pointer graph_pointer = -1;
            int size = -1;
            in.read(reinterpret_cast<char *>(&size), sizeof(size));
            for (int i = 0; i < size; ++i) {
                in.read(reinterpret_cast<char *>(&graph_pointer), sizeof(graph_pointer));
                PEGraph *pegraph = new PEGraph();

                pegraph->load_unreadable(in);
                if (map.find(graph_pointer) != map.end()) {
                    delete map[graph_pointer];
                }
                map[graph_pointer] = pegraph;
            }
        }
    }


    //deep copy; locked version for asynchronous mode
    PEGraph *retrieve_locked(PEGraph_Pointer graph_pointer) {
        std::lock_guard<std::mutex> lockGuard(mutex);
        return retrieve(graph_pointer);
    }

    //deep copy
    PEGraph *retrieve(PEGraph_Pointer graph_pointer) {
//    	//for debugging
//    	Logger::print_thread_info_locked("retrieve starting...\n", LEVEL_LOG_FUNCTION);

        PEGraph *out;

        if (map.find(graph_pointer) != map.end()) {
//    		Logger::print_thread_info_locked("retrieving +++++++++++++++++++++++ " +  to_string((long) map[graph_pointer]) + " +++++++++++++++++++++++\n", LEVEL_LOG_GRAPHSTORE) ;
            out = new PEGraph(map[graph_pointer]);
        } else {
//    		out = new PEGraph();
            out = nullptr;
        }

        //for debugging
//    	Logger::print_thread_info_locked("retrieve finished.\n", LEVEL_LOG_FUNCTION);

        return out;
    }

    //deep copy; locked version for asynchronous mode
    void update_locked(PEGraph_Pointer graph_pointer, PEGraph *pegraph) {
        std::lock_guard<std::mutex> lockGuard(mutex);

        auto start_fsm = std::chrono::high_resolution_clock::now();

        update(graph_pointer, pegraph);

        auto end_fsm = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> diff_fsm = end_fsm - start_fsm;
        myTimer::addDurationUpdate(diff_fsm.count());
        myTimer::addCountUpdate();
    }

    //deep copy
    void update(PEGraph_Pointer graph_pointer, PEGraph *pegraph) {
//    	//for debugging
//    	Logger::print_thread_info_locked("update starting...\n", LEVEL_LOG_FUNCTION);

//    	assert(map.find(graph_pointer) != map.end());
        if (map.find(graph_pointer) != map.end()) {
//    		Logger::print_thread_info_locked("deleting +++++++++++++++++++++++ " +  to_string((long) map[graph_pointer]) + " +++++++++++++++++++++++\n", LEVEL_LOG_GRAPHSTORE) ;
            delete map[graph_pointer];
        }
        map[graph_pointer] = new PEGraph(pegraph);

        //for debugging
//    	Logger::print_thread_info_locked("update finished.\n", LEVEL_LOG_FUNCTION);
    }


    void addOneGraph_atomic(PEGraph_Pointer pointer, PEGraph *graph) {
        std::lock_guard<std::mutex> lockGuard(mutex);
        this->map[pointer] = graph;
    }

    static void
    update_parallel(NaiveGraphStore *current, NaiveGraphStore *another, Concurrent_Worklist<vertexid_t> *worklist) {
        vertexid_t id = -1;
        while (worklist->pop_atomic(id)) {
            assert(current->map.find(id) != current->map.end());

            auto start_fsm = std::chrono::high_resolution_clock::now();

            current->update(id, another->map.at(id));

            auto end_fsm = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff_fsm = end_fsm - start_fsm;
            myTimer::addDurationUpdate(diff_fsm.count());
            myTimer::addCountUpdate();
        }
    }

    void update_graphs_parallel(GraphStore *another) {
        //initiate concurrent worklist
        Concurrent_Worklist<vertexid_t> *worklist = new Concurrent_Workset<vertexid_t>();
        NaiveGraphStore *another_graphstore = dynamic_cast<NaiveGraphStore *>(another);
        for (auto &it: another_graphstore->map) {
            worklist->push_atomic(it.first);

            //initialize the graphstore
            if (map.find(it.first) == map.end()) {
                map[it.first] = new PEGraph();
            }
        }

        std::vector<std::thread> comp_threads;
        for (unsigned int i = 0; i < NUM_THREADS; i++)
            comp_threads.push_back(std::thread([=] { update_parallel(this, another_graphstore, worklist); }));

        for (auto &t : comp_threads)
            t.join();

        //clean
        delete (worklist);
    }

    void update_graphs_sequential(GraphStore *another) {
        NaiveGraphStore *another_graphstore = dynamic_cast<NaiveGraphStore *>(another);
        for (auto &it: another_graphstore->map) {

            auto start_fsm = std::chrono::high_resolution_clock::now();

            update(it.first, it.second);

            auto end_fsm = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff_fsm = end_fsm - start_fsm;
            myTimer::addDurationUpdate(diff_fsm.count());
            myTimer::addCountUpdate();
        }
    }

    void update_graphs(GraphStore *another) {
//    	update_graphs_sequential(another); // sequential
        update_graphs_parallel(another); // in parallel
    }

    void clearEntryOnly() {
        this->map.clear();
    }

    void clear() {
        for (auto it = map.begin(); it != map.end();) {
            delete it->second;
            it = map.erase(it);
        }
    }


    void printOutInfo() {
        int size_graphs = map.size();
        long size_edges = 0;

        cout << "GraphStore Info >>>>>>>>>>>>>>>>>>>>>>>>>>>" << endl;

        for (auto it = map.begin(); it != map.end(); ++it) {
//    		cout << it->first << "\t" << it->second->getNumEdges() << endl;
            size_edges += it->second->getNumEdges();
        }

        cout << "Number of graphs: " << size_graphs << endl;
        cout << "Number of edges: " << size_edges << endl;
    }


    std::unordered_map<PEGraph_Pointer, PEGraph *> &getMap() {
        return map;
    }


protected:
    void print(std::ostream &str) {
        std::lock_guard<std::mutex> lockGuard(mutex);
        str << "The number of graphs is: " << map.size() << "\n";
        for (auto it = map.begin(); it != map.end(); ++it) {
            str << ">>>>" << it->first << " " << *(it->second) << endl;
        }
    }

    void toString_sub(std::ostringstream &str) {
        std::lock_guard<std::mutex> lockGuard(mutex);
        str << "The number of graphs is: " << map.size() << "\n";
        for (auto it = map.begin(); it != map.end(); ++it) {
            str << ">>>>" << it->first << " " << *(it->second) << endl;
        }
    }


private:
    std::unordered_map<PEGraph_Pointer, PEGraph *> map;

    enum STATUS {
        ROOT_NULL = 0x0,
        ROOT = 0x1,
        LEFT = 0x2,
        RIGHT = 0x4
    };

};


#endif /* COMP_GRAPHSTORE_NAIVE_GRAPHSTORE_H_ */
