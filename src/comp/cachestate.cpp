//
// Created by decxu on 3/10/21.
//

#include "cachestate.h"

cachestate::cachestate(unsigned lineSize, unsigned lineNum, unsigned setNum) {


    auto isPowerOf2 = [](unsigned x)->bool {return (x&(x - 1)) == 0; };
    if (!(isPowerOf2(lineSize) && isPowerOf2(setNum)) && (lineNum%setNum == 0))
        cout << "Error: invalid cache configuration!\n" << endl;


    CacheLineNum = lineNum; //cache 行数
    CacheLineSize = lineSize;  //cache 每行的大小
    CacheSetNum = setNum; //cache 组相联数
    CacheLinesPerSet = lineNum / setNum;// cache每组行数
    HitCount_Icache = MissCount_Icache = 0;
    count = 0;

    for (int i = 0; i < 128; i++) {
        std::map<int, Line> newSet;
        IRs_Icache.push_back(newSet);
    }
}

void cachestate::setInstsMap(std::vector<std::map<int, int>> insts) {
    for (int i = 0; i < this->IRs_Icache.size(); i++) {
        IRs_Icache[i].empty();
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

    //在所在的组内进行查找
    int setNum = getSetNum(ir);
    unsigned beginLine = setNum * 4;
    //cout << "  " << setNum << endl;

    std::map<int, int> *IRs_Set = &(this->IRs_Icache[setNum]);


    if (IRs_Set->find(ir) != IRs_Set->end()) {//case1: 在该set中，则为hit的情况


        flag = 1; //在cache中

        /*	需要更新，所在set的age信息，时间访问为常量	*/


        /*  根据组相联，hit时:
        *	1) 先找到所在组
        *	2) 自己的age更新为0
        *	3) 再找到所在组中，age小于其的行，行号++
        */

        int CacheLoc = (*IRs_Set)[ir]; //行号
        int age = CacheLoc % 128; //取模对应真实的age

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

    else { //case2: 访问该mem IR，但不在其对应的cache  set中
        flag = 0;
        //printf("Fatal: try to access IR, but not added to Mem-Cache map!\n\n");

        /*  根据组相联，miss时:
        *	1) 先找到所在组
        *	2) 构建映射关系，特别是对应到哪一行
        *	3) 剩余变量的行号++，大于组内行数则移出该组
        */

        //newInst->Line = beginLine;  //1) 初次进入组内时，需设置行号，且为该组第0行
        (*IRs_Set)[ir] = beginLine;


        /*
        *  1) 设置为组内第0行
        *  2) mem与cache的映射表发生更新
        */
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

    /*
    * 两model求交集，只有当变量在两个model中都存在时，才会在merge后的model中
    */

    /*
    * 1）先扫描被合并的model
    */
    for (int j = 0; j < mod->IRs_Icache.size(); j++) {

        std::map<int, int> *Set1 = &(mod->IRs_Icache[j]);
        std::map<int, int> *Set2 = &(this->IRs_Icache[j]);

        for (auto item : *Set1) {
            int val = item.first;
            if ((*Set2).find(val) != (*Set2).end()) {
                unsigned line2 = (*Set2)[val];
                unsigned line1 = (*Set1)[val];
                unsigned maxLine = line2 > line1 ? line2 : line1;
                (*Set2)[val] = maxLine;

            }
        }
    }

    /*
    * 2）再扫描，当前model
    */
    for (int j = 0; j < mod->IRs_Icache.size(); j++) {
        std::map<int, int> *Set1 = &(mod->IRs_Icache[j]);
        std::map<int, int> *Set2 = &(this->IRs_Icache[j]);

        std::map<int, int>::iterator iter = (*Set2).begin();
        for (; iter != (*Set2).end();) {
            int val = (*iter).first;
            if ((*Set1).find(val) == (*Set1).end()) {
                (*Set2).erase(iter++);
                //printf("not exit in both model!\n\n");
            }
            else {
                ++iter;
            }
        }
    }

    this->MissCount_Icache = this->MissCount_Icache > mod->MissCount_Icache ? this->MissCount_Icache : mod->MissCount_Icache; //合并后，传递来的miss数取前继最大
    return this;
}

cachestate* cachestate::fork() {

    cachestate *ret = new cachestate(this->CacheLineSize, this->CacheLineNum, this->CacheSetNum);

    /* similar for i-cache addr count */
    ret->setInstsMap(this->IRs_Icache); //ret->setInstsMap(this->IRs_Icache);

    /* similar for i-cache hit/miss and specu hit/miss count */
    ret->HitCount_Icache = this->HitCount_Icache;
    ret->MissCount_Icache = this->MissCount_Icache;

    return ret;
}