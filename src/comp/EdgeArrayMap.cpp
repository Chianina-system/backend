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

int EdgeArrayMap::getNum() const {
    return num;
}

void EdgeArrayMap::setNum(int num) {
    EdgeArrayMap::num = num;
}

void EdgeArrayMap::setEdgeArray(vertexid_t index, EdgeArray *edgeArray) {
    edgeArray_Map[index] = edgeArray;
    num = edgeArray_Map.bucket_count();
}

EdgeArray *EdgeArrayMap::getEdgeArray(vertexid_t index) {
    if (edgeArray_Map.find(index)== edgeArray_Map.end())
        return nullptr;
    else return edgeArray_Map[index];
}
