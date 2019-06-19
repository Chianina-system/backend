//
// Created by guihang on 2019/6/16.
//

#include "EdgeArrayMap.h"

EdgeArrayMap::EdgeArrayMap(const std::unordered_map<vertexid_t, EdgeArray *> &edgeArrayMap) : edgeArray_Map(
        edgeArrayMap) {}

EdgeArrayMap::EdgeArrayMap() {}

EdgeArrayMap::~EdgeArrayMap() {

}

const std::unordered_map<vertexid_t, EdgeArray *> &EdgeArrayMap::getEdgeArrayMap() const {
    return edgeArray_Map;
}

void EdgeArrayMap::setEdgeArrayMap(const std::unordered_map<vertexid_t, EdgeArray *> &edgeArrayMap) {
    edgeArray_Map = edgeArrayMap;
}

int EdgeArrayMap::getNum() const {
    return num;
}

void EdgeArrayMap::setNum(int num) {
    EdgeArrayMap::num = num;
}