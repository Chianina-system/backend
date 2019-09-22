#include "edgearray.h"


EdgeArray::EdgeArray() {
    edges = NULL;
    labels = NULL;
    size = capacity = 0;
}


//copy constructor
EdgeArray::EdgeArray(const EdgeArray& array){
	size = array.size;
	capacity = array.capacity;
	if(size){
		this->edges = new vertexid_t[size];
		this->labels = new label_t[size];
        memcpy(this->edges,array.edges,sizeof(vertexid_t)*size);
        memcpy(this->labels,array.labels,sizeof(label_t)*size);
	}
	else{
		edges = NULL;
		labels = NULL;
	}

}

//copy assign operator
EdgeArray& EdgeArray::operator=(const EdgeArray& array){
//	//for debugging
//	Logger::print_thread_info_locked("edgearray copy starting...\n", LEVEL_LOG_FUNCTION);

	size = array.size;
	capacity = array.capacity;
	if(size){
		this->edges = new vertexid_t[size];
		this->labels = new label_t[size];
        memcpy(this->edges,array.edges,sizeof(vertexid_t)*size);
        memcpy(this->labels,array.labels,sizeof(label_t)*size);
	}
	else{
		edges = NULL;
		labels = NULL;
	}

//	//for debugging
//	Logger::print_thread_info_locked("edgearray copy finished.\n", LEVEL_LOG_FUNCTION);

	return *this;
}

//EdgeArray::EdgeArray(int size,vertexid_t *edges,label_t *labels) {
//    this->size = size;
//    if(size) {
//        this->edges = new vertexid_t[size];
//        this->labels = new label_t[size];
//        memcpy(this->edges,edges,sizeof(vertexid_t)*size);
//        memcpy(this->labels,labels,sizeof(label_t)*size);
//    }
//    this->capacity = size;
//}

void EdgeArray::set(int size, vertexid_t *edges, label_t *labels) {
    if(!size)
        return;

    if(!this->size) {
        this->size = size;
        this->edges = new vertexid_t[size];
        this->labels = new label_t[size];
        memcpy(this->edges,edges,sizeof(vertexid_t)*size);
        memcpy(this->labels,labels,sizeof(label_t)*size);
    }
    else {
        if(this->size != size) {
            if(this->edges)	delete[] this->edges;
            if(this->labels) delete[] this->labels;
            this->edges = new vertexid_t[size];
            this->labels = new label_t[size];
        }
        this->size = size;
        memcpy(this->edges,edges,sizeof(vertexid_t)*size);
        memcpy(this->labels,labels,sizeof(label_t)*size);
    }
}
/*
void EdgeArray::print() {
	if(!size)
		cout << "empty edgearray! " << endl;
	else {
		for(int i = 0;i < size;++i) {
			cout << edges[i] << ", " << (int)labels[i] << " -> ";
		 }
	cout << "end" << endl;
	}
}
*/

void EdgeArray::clear() {
    if(size) {
        if(edges) {
            delete[] edges;
            edges = NULL;
        }
        if(labels) {
            delete[] labels;
            labels = NULL;
        }
        size = capacity = 0;
    }
}

void EdgeArray::addOneEdge(vertexid_t edge, label_t label) {
    if(size == 0) {
        capacity = CAPACITY_VALUE;
        edges = new vertexid_t[capacity];
        labels = new label_t[capacity];
        //initial
        for(int i = 0;i < capacity;++i) {
            edges[i] = -1;
            labels[i] = (char)127;
        }
    }
    else {
        if(size >= capacity) {
            capacity *= 2;
            myalgo::myrealloc(edges,size,capacity);
            myalgo::myrealloc(labels,size,capacity);
            for(int i = size;i < capacity;++i) {
                edges[i] = -1;
                labels[i] = (char)127;
            }
        }
    }
    // add edge
    edges[size] = edge;
    labels[size] = label;
    ++size;
}

//void EdgeArray::setRealNumEdges(int realNumEdges) {
//    this->realNumEdges = realNumEdges;
//}

void EdgeArray::addEdges(int len, vertexid_t *_edges, label_t *_labels) {
    if (len == 0) return;
    if (len == 1){
        addOneEdge(_edges[0], _labels[0]);
        return;
    }
    int realSize = size + len;
    while (realSize >= capacity){
        capacity *= 2;
    }
    myalgo::myrealloc(edges, size, capacity);
    myalgo::myrealloc(labels, size, capacity);
    for (int i = size; i < realSize; ++i) {
        edges[i] = _edges[i-size];
        labels[i] = _labels[i-size];
    }
    for (int i = realSize; i < capacity; ++i) {
        edges[i] = -1;
        labels[i] = (char) 127;
    }
}

void EdgeArray::merge() {
    // sort edges
    myalgo::quickSort(edges, labels, 0, size - 1);
    // remove duplicate edges
    vertexid_t *_edges = new vertexid_t[size];
    label_t *_labels = new label_t[size];
    int _numEdges = 0;
    myalgo::removeDuple(_numEdges, _edges, _labels, size, edges, labels);
    size = _numEdges;
    memcpy(edges, _edges, sizeof(vertexid_t) * _numEdges);
    memcpy(labels, _labels, sizeof(label_t) * _numEdges);
    delete[] _edges;
    delete[] _labels;
}

bool EdgeArray::equals(EdgeArray* another) const {
	if(!another){
		return false;
	}

	if(this == another){
		return true;
	}

	if(this->getSize() != another->getSize()){
		return false;
	}

	if(memcmp(this->labels, another->labels, sizeof(label_t)*size)){
		return false;
	}

	if(memcmp(this->edges, another->edges, sizeof(vertexid_t)*size)){
		return false;
	}

	return true;
}


