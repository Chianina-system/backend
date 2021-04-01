//
// Created by decxu on 3/10/21.
//

#ifndef BACKEND_TUNING_CACHESTATE_H
#define BACKEND_TUNING_CACHESTATE_H
#include "../common/CommonLibs.hpp"
class cachestate {
public:
    unsigned CacheLineNum;
    unsigned CacheLineSize;
    unsigned CacheSetNum;
    unsigned CacheLinesPerSet;

    std::vector<std::map<int, Line>> IRs_Icache; //大小为Set. 每组内包含，缓存到该组的IRs

    unsigned HitCount_Icache, MissCount_Icache;

    int count;



    cachestate(unsigned lineSize, unsigned lineNum, unsigned setNum);

    bool CacheConsistent(cachestate* model) {

        for (int k = 0; k < 128; k++) {

            if (this->IRs_Icache[k].size() != model->IRs_Icache[k].size()) {
                return false;
            }

            //vector<map<string, Inst*>> IRs_Icache;
            for (auto item : this->IRs_Icache[k]) {
                if (model->IRs_Icache[k].find(item.first) == model->IRs_Icache[k].end()) {
                    return false;
                }

                if (this->IRs_Icache[k][item.first] != model->IRs_Icache[k][item.first]) {
                    return false;
                }
            }
        }
        //cout << "is same!" << endl;
        return true;
    }

    void setInstsMap(std::vector<std::map<int, Line>>);

    cachestate* fork();

    cachestate* merge(cachestate* mod);

    unsigned AccessIC(int ir);

    int getSetNum(int addr) {

        int sum = addr / 64;
        sum = sum % 128;


        return sum;
    }

    size_t compute_size_bytes(){
        size_t size = 0;
        size += 2 * sizeof(unsigned);
        size += sizeof(int);
        size += sizeof(std::vector<std::map<int, Line>>::size_type);
        for (int i = 0; i < IRs_Icache.size(); ++i) {
            size += sizeof(std::map<int, Line>::size_type);
            for (int j = 0; j < IRs_Icache[i].size(); ++j) {
                size += sizeof(int);
                size += sizeof(Line);
            }
        }
        return size;
    }

    void write_to_buf(char* buf, PEGraph_Pointer pointer){
        size_t offset = 0;
        memcpy(buf, (char*)&pointer, sizeof(PEGraph_Pointer));
        offset += sizeof(PEGraph_Pointer); // 写一个id

        memcpy(buf + offset, (char*)&HitCount_Icache, sizeof(unsigned));
        offset += sizeof(unsigned); // 写一个hit

        memcpy(buf + offset, (char*)&MissCount_Icache, sizeof(unsigned));
        offset += sizeof(unsigned); // 写一个miss

        memcpy(buf + offset, (char*)&count, sizeof(int));
        offset += sizeof(int); // 写一个count

        std::vector<std::map<int, Line>>::size_type vector_size = IRs_Icache.size();
        memcpy(buf + offset, (char*)&vector_size, sizeof(std::vector<std::map<int, Line>>::size_type));
        offset += sizeof(std::vector<std::map<int, Line>>::size_type);// 写vector的大小

        for (int i = 0; i < IRs_Icache.size(); ++i) {
            std::map<int, Line>::size_type size_map = IRs_Icache[i].size();
            memcpy(buf + offset, (char*)&size_map, sizeof(std::map<int, Line>::size_type));
            offset += sizeof(std::map<int, Line>::size_type);// 写map的大小

            for(auto &it : IRs_Icache[i]){
                //cout << it.first << "-" << it.second << endl;
                memcpy(buf + offset, (char*)&it.first, sizeof(int));
                offset += sizeof(int); // 写一个key
                memcpy(buf + offset, (char*)&it.second, sizeof(Line));
                offset += sizeof(Line); // 写一个value
            }
        }
//        for(auto& it: graph){
//            //srcId
//            memcpy(buf + offset, (char*)&(it.first), sizeof(vertexid_t));
////    		*(buf + offset) = it.first;
//            offset += sizeof(vertexid_t);
//            //edgeArray
//            offset = it.second.write_to_buf(buf, offset);
//        }
    }

    PEGraph_Pointer read_from_buf(char* buf, const size_t bufsize){
        size_t offset = 0;
        PEGraph_Pointer pointer = *((PEGraph_Pointer*)(buf));
        offset += sizeof(PEGraph_Pointer); //读一个id

        HitCount_Icache = *((unsigned*)(buf + offset));
        offset += sizeof(unsigned); //读一个hit

        MissCount_Icache = *((unsigned*)(buf + offset));
        offset += sizeof(unsigned); //读一个miss

        count = *((int*)(buf + offset));
        offset += sizeof(int); //读一个miss

        std::vector<std::map<int, Line>>::size_type vector_size = *((std::vector<std::map<int, Line>>::size_type*)(buf + offset));
        offset += sizeof(std::vector<std::map<int, Line>>::size_type); //读一个vector大小

        for (int i = 0; i < vector_size; ++i) {
            std::map<int, Line>::size_type map_size = *((std::map<int, Line>::size_type*)(buf + offset));
            offset += sizeof(std::map<int, Line>::size_type); //读一个map大小
            for (int j = 0; j < map_size; ++j) {
                int key = *((int*)(buf + offset));
                offset += sizeof(int); //读一个key
                Line value = *((Line*)(buf + offset));
                offset += sizeof(Line); //读一个value
                IRs_Icache[i][key] = value;
            }
        }

//        while(offset < bufsize){
//            vertexid_t src = *((vertexid_t*)(buf + offset));
//            offset += sizeof(vertexid_t);
//            this->graph[src] = EdgeArray();
//            offset = this->graph[src].read_from_buf(buf, offset);
//        }
        return pointer;
    }

    void print(){
        //cout << "IRs_Icache size:" << IRs_Icache.size() << endl;
        cout << "map size: " ;
        for (int i = 0; i < IRs_Icache.size(); i++) {
            if (IRs_Icache[i].size() != 0)
            {
                cout << "vector index: " << i << " size: " << IRs_Icache[i].size() << " " << endl;
                std::map<int, int>::iterator iter;
                for(iter = IRs_Icache[i].begin(); iter != IRs_Icache[i].end(); iter++) {
                    cout << iter->first << " : " << iter->second << "  " << endl;
                }
            }
        }
    }

};


#endif //BACKEND_TUNING_CACHESTATE_H
