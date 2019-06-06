#include "edgearray.h"

EdgeArray::EdgeArray() {
	this->size = 0;				
}

EdgeArray::EdgeArray(int size,vertexid_t *edges,label_t *labels) {
	this->size = size;
	if(size) {
		this->edges = new vertexid_t[size];
		this->labels = new label_t[size];
		memcpy(this->edges,edges,sizeof(vertexid_t)*size);
		memcpy(this->labels,labels,sizeof(label_t)*size);
	}
}

void EdgeArray::set(int size,vertexid_t *edges,label_t *labels) {
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
		size = 0;
	}	
}
