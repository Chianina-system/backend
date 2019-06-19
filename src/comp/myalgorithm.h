#ifndef MYALGORITHM_H
#define MYALGORITHM_H
#include "../common/CommonLibs.hpp"

namespace myalgo {

    inline int myCompare(vertexid_t v1,label_t l1,vertexid_t v2,label_t l2) {return (v1 == v2) ? (l1 - l2) : (v1 - v2);}

// sort algorithm
    void quickSort(vertexid_t *A,char *B,int l,int r);
    void insertSort(vertexid_t *A,char *B,int l,int r);
    int split(vertexid_t *A,char *B,int l,int r);

// merge sorted arrays algorithm
    void unionTwoArray(int &len,vertexid_t *dstA,char *dstB,int len1,vertexid_t *A1,char *B1,int len2,vertexid_t *A2,char *B2); // union set
    void minusTwoArray(int &len,vertexid_t *dstA,char *dstB,int len1,vertexid_t *A1,char *B1,int len2,vertexid_t *A2,char *B2); // difference set

    void removeDuple(int &len,vertexid_t *dstA,char *dstB,int srclen,vertexid_t *srcA,char *srcB); // remove duplicate edges
    bool checkEdges(int len,vertexid_t *A,char *B); // check duplicate edges and sequence

    template <typename T>
    void myrealloc(T* &arr,int size,int Tosize) {
        T* tmpArr;
        tmpArr = new T[Tosize];

        for(int i = 0;i < size;++i) {
            tmpArr[i] = arr[i];
        }
        delete[] arr;
        arr = tmpArr;
    }

};
#endif
