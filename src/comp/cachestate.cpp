//
// Created by decxu on 3/10/21.
//

#include "cachestate.h"

cachestate::cachestate() {
    // std::map<int, Line>* IRs_Icache
    IRs_Icache = new std::map<int, Line>[CacheLinesPerSet];
    for (int i = 0; i < CacheLinesPerSet; i++) {
        std::map<int, Line> newSet;
        IRs_Icache[i] = newSet;
    }
    out_initialized = false;
}

cachestate::cachestate(bool initialized) {
    out_initialized = true;
}

void cachestate::setInstsMap(std::map<int, int>* insts) {
    for (int i = 0; i < CacheLinesPerSet; i++) {
        IRs_Icache[i].clear();
        for (auto item : insts[i]) {
            IRs_Icache[i][item.first] = item.second;
        }
    }
}

unsigned cachestate::AccessIC(int ir) {

    /*
    *	case1: flag 为-1，表示cache的非正常访问（初始状态）
    *	case2: flag 为1，IR在cache内，hit++，age变化
    *	case3: flag 为0，IR在cache外，miss++，map表与age都需变化
    */
    unsigned flag = -1;

    // find in corresponding set
    int setNum = getSetNum(ir);
    unsigned beginLine = setNum * 4;

    std::map<int, int> *IRs_Set = &(this->IRs_Icache[setNum]);


    if (IRs_Set->find(ir) != IRs_Set->end()) {//case1: find in set, so is hit
        flag = 1; // find in cache


        int CacheLoc = (*IRs_Set)[ir]; //行号
        int age = CacheLoc % CacheLinesPerSet; //取模对应真实的age

        for (auto item : *IRs_Set) {
            if (item.first == ir) {
                (*IRs_Set)[ir] = beginLine; //返回到该组的第一行
            }
            else if (item.second < CacheLoc) { //组内在其前面个的行，变量都会后移一行
                (*IRs_Set)[item.first] = item.second+1;
            }
        }

        std::map<int, int>::iterator iter = (*IRs_Set).begin();
        for (; iter != (*IRs_Set).end();) {
            if ((*iter).second > beginLine + 3) {
                (*IRs_Set).erase(iter++);
            }
            else {
                ++iter;
            }
        }

    }

    else { // case2:not in corresponding set
        flag = 0;
        (*IRs_Set)[ir] = beginLine;

        for (auto item : *IRs_Set) {
            if (item.first != ir) {
                (*IRs_Set)[item.first] = item.second + 1;//2) 旧变量的行号都得+1
            }
        }
        std::map<int, int>::iterator iter = (*IRs_Set).begin();
        for (; iter != (*IRs_Set).end();) {
            if ((*iter).second > beginLine + 3) {
                iter = (*IRs_Set).erase(iter);
            }
            else {
                iter++;
            }
        }

    }

    return flag;	// return 1, if var in cache, else 0. use return value to count cache hits/miss
}

cachestate* cachestate::merge(cachestate* mod) {

    if (mod == nullptr)
        return this;

    for (int j = 0; j < CacheLinesPerSet; j++) {

        std::map<int, int> *Set1 = &(mod->IRs_Icache[j]);
        std::map<int, int> *current_set = &(this->IRs_Icache[j]);

        std::map<int, int>::iterator iter = (*Set1).begin(); //scan the other state to find may var
        for (; iter != (*Set1).end();) {

            int val = (*iter).first;
            if ((*current_set).find(val) != (*current_set).end()) { //mod中存在，line取最大值
                unsigned line2 = (*Set1)[val];
                unsigned line1 = (*current_set)[val];
                unsigned minLine = line2 < line1 ? line2 : line1;
                (*current_set)[val] = minLine;
                ++iter;
            }
            else { // not in, due to may, so add it
                // (*Set2).erase(iter++);
                (*current_set)[val] = (*Set1)[val];
            }
        }
    }

    return this;
}

cachestate* cachestate::fork() {

    cachestate *ret = new cachestate();

    ret->setInstsMap(this->IRs_Icache);

    return ret;
}