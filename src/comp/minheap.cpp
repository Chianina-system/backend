#include <limits.h>
#include "minheap.h"

MinHeap::MinHeap(MinHeapNode a[],int size) {	
	this->size = size;
	harr = a;
	int i = (size-1)/2;
	while(i >= 0) {
		MinHeapify(i);
		--i;
	}
}

void MinHeap::MinHeapify(int i) {
	int l = left(i);
	int r = right(i);
	int smallest = i;
	if(l < size && myalgo::myCompare(harr[l].key_v,harr[l].key_c,harr[i].key_v,harr[i].key_c) < 0)
		smallest = l;
	if(r < size && myalgo::myCompare(harr[r].key_v,harr[r].key_c,harr[smallest].key_v,harr[smallest].key_c) < 0)
		smallest = r;
	if(smallest != i) {
		swap(&harr[i],&harr[smallest]);
		MinHeapify(smallest);
	}
}

void MinHeap::replaceMin(MinHeapNode p) {
	harr[0] = p;
	MinHeapify(0);
}

void MinHeap::swap(MinHeapNode *x,MinHeapNode *y) {
	MinHeapNode tmp = *x; *x = *y; *y = tmp;		
}
