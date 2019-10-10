#include "myalgorithm.h"

namespace myalgo {

    void insertSort(vertexid_t *A,char *B,int l,int r) {
        for(int j = l+1;j <= r;++j) {
            vertexid_t key_v = A[j];
            char key_c = B[j];
            int i = j-1;
            while(i >= l && (key_v < A[i] || (key_v == A[i] && key_c < B[i]))) {
                A[i+1] = A[i];
                B[i+1] = B[i];
                --i;
            }
            A[i+1] = key_v;
            B[i+1] = key_c;
        }
    }

    void insertSortArray(int* arr, unsigned int n){
        int i, key, j;
        for (i = 1; i < n; i++) {
            key = arr[i];
            j = i - 1;

            /* Move elements of arr[0..i-1], that are
              greater than key, to one position ahead
              of their current position */
            while (j >= 0 && arr[j] > key) {
                arr[j + 1] = arr[j];
                j = j - 1;
            }
            arr[j + 1] = key;
        }
    }

    void quickSort(vertexid_t *A,char *B,int l,int r) {
        if(l < r) {
            if(r - l + 1 <= 10)
                insertSort(A,B,l,r);
            else {
                int i = split(A,B,l,r);
                quickSort(A,B,l,i-1);
                quickSort(A,B,i+1,r);
            }
        }
    }

    int split(vertexid_t *A,char *B,int l,int r) {
        int mid = (l + r) / 2; int k = l;
        if(A[mid] < A[k]) k = mid;
        if(A[r] < A[k]) k = r;
        if(k != r) {
            std::swap(A[k],A[r]);
            std::swap(B[k],B[r]);
        }
        if(mid != l && A[mid] < A[l]) {
            std::swap(A[mid],A[l]);
            std::swap(B[mid],B[l]);
        }
        vertexid_t val_v = A[l];
        char val_c = B[l];

        int i = l;
        for(int j = l+1; j <= r; ++j) {
            if((A[j] < val_v) || (A[j] == val_v && B[j] < val_c)) {
                ++i;
                std::swap(A[i],A[j]);
                std::swap(B[i],B[j]);
            }
        }
        std::swap(A[i],A[l]);
        std::swap(B[i],B[l]);
        return i;
    }

    int unionTwoArray(vertexid_t *dstA,char *dstB,int len1,vertexid_t *A1,char *B1,int len2,vertexid_t *A2,char *B2) {
        // (A1,B1),(A2,B2) is sorted
        int len = 0;
        if(len1) {
            if(len2) {
                int p1 = 0; int p2 = 0;
                while(p1 < len1 && p2 < len2) {
                    int value = myCompare(A1[p1],B1[p1],A2[p2],B2[p2]);
                    if(value > 0) {
                        dstA[len] = A2[p2]; dstB[len] = B2[p2];
                        ++p2; ++len;
                    }
                    else if(value < 0) {
                        dstA[len] = A1[p1]; dstB[len] = B1[p1];
                        ++p1; ++len;
                    }
                    else {
                        dstA[len] = A1[p1]; dstB[len] = B1[p1];
                        ++p1; ++p2; ++len;
                    }
                }
                while(p1 < len1) {
                    dstA[len] = A1[p1]; dstB[len] = B1[p1];
                    ++p1; ++len;
                }
                while(p2 < len2) {
                    dstA[len] = A2[p2]; dstB[len] = B2[p2];
                    ++p2; ++len;
                }
            }
            else {
                len = len1;
                memcpy(dstA,A1,sizeof(vertexid_t)*len);
                memcpy(dstB,B1,sizeof(char)*len);
            }
        }
        else {
            if(len2) {
                len = len2;
                memcpy(dstA,A2,sizeof(vertexid_t)*len);
                memcpy(dstB,B2,sizeof(char)*len);
            }
        }

        return len;

    }

    int minusTwoArray(vertexid_t *dstA,char *dstB,int len1,vertexid_t *A1,char *B1,int len2,vertexid_t *A2,char *B2) {
        // (A1,B1),(A2,B2) is sorted
        int len = 0;
        if(len1) {
            if(len2) {
                int p1 = 0; int p2 = 0;
                while(p1 < len1 && p2 < len2) {
                    int value = myCompare(A1[p1],B1[p1],A2[p2],B2[p2]);
                    if(value > 0) {
                        ++p2;
                    }
                    else if(value < 0) {
                        dstA[len] = A1[p1]; dstB[len] = B1[p1];
                        ++p1; ++len;
                    }
                    else {
                        ++p1; ++p2;
                    }
                }
                while(p1 < len1) {
                    dstA[len] = A1[p1]; dstB[len] = B1[p1];
                    ++p1; ++len;
                }
            }
            else {
                len = len1;
                memcpy(dstA,A1,sizeof(vertexid_t)*len);
                memcpy(dstB,B1,sizeof(char)*len);
            }
        }

        return len;
    }

    void removeDuple(int &len,vertexid_t *dstA,char *dstB,int srclen,vertexid_t *srcA,char *srcB) {
        if(srclen) {
            *dstA = *srcA; *dstB = *srcB;
            len = 1;
            for(int i = 1;i < srclen;++i) {
                if(srcA[i] == srcA[i-1] && srcB[i] == srcB[i-1])
                    continue;
                else {
                    dstA[len] = srcA[i];
                    dstB[len] = srcB[i];
                    ++len;
                }
            }
        }
        else {
            len = 0;
        }
    }

    bool checkEdges(int len,vertexid_t *A,char *B) {
        if(len <= 1)
            return false;
        else {
            for(int i = 1;i < len;++i) {
                if(A[i-1] > A[i]) {
                    return true;
                }
                if(A[i-1] == A[i] && B[i-1] >= B[i]) {
                    return true;
                }
            }
            return false;
        }
    }

}
