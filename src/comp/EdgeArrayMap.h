//
// Created by guihang on 2019/6/16.
//

#ifndef BACKEND_EDGEARRAYMAP_H
#define BACKEND_EDGEARRAYMAP_H

#include "edgearray.h"
#include "../common/CommonLibs.hpp"
class EdgeArrayMap {
public:
    EdgeArrayMap(const std::unordered_map<vertexid_t, EdgeArray *> &edgeArrayMap);

    EdgeArrayMap();

    virtual ~EdgeArrayMap();

    const std::unordered_map<vertexid_t, EdgeArray *> &getEdgeArrayMap() const;

    EdgeArray* getEdgeArray(vertexid_t index);

    void setEdgeArray(vertexid_t index,EdgeArray* edgeArray);

    int getNum() const;

    void setNum(int num);



private:
    std::unordered_map<vertexid_t, EdgeArray*> edgeArray_Map;
    int num;
};


#endif //BACKEND_EDGEARRAYMAP_H
