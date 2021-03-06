/*
 * edge.h
 *
 *  Created on: Sep 21, 2019
 *      Author: dell
 */

#ifndef COMP_GRAPHSTORE_EDGE_H_
#define COMP_GRAPHSTORE_EDGE_H_

#include "../../common/CommonLibs.hpp"
#include "../../utility/uintseqhash.hh"

class Edge {

	friend std::ostream & operator<<(std::ostream & strm, const Edge & edge) {
		strm << edge.srcId << ", " << edge.dstId << ", " << (int)edge.label << endl;
		return strm;
	}

public:
	Edge(vertexid_t src, vertexid_t dst, label_t l){
		srcId = src;
		dstId = dst;
		label = l;
	}

	~Edge(){

	}

	inline vertexid_t getSrcId() const {
		return srcId;
	}

	unsigned int get_hash() const{
//		return ((srcId << 1) + (dstId >> 1) + label);
		bliss::UintSeqHash h;

		h.update(srcId);
		h.update(dstId);
		h.update(label);

		return h.get_value();
	}

	bool operator==(const Edge& other) const {
		return srcId == other.srcId && dstId == other.dstId && label == other.label;
	}

	inline vertexid_t getDstId() const {
		return dstId;
	}

	inline label_t getLabel() const {
		return label;
	}

	static size_t getSizeof() {
		return sizeof(vertexid_t) * 2 + sizeof(label_t);
	}

	long write_to_buf(char* buf, size_t offset){
    	memcpy(buf + offset, (char*)& srcId, sizeof(vertexid_t));
    	offset += sizeof(vertexid_t);
    	memcpy(buf + offset, (char*)& dstId, sizeof(vertexid_t));
    	offset += sizeof(vertexid_t);
    	memcpy(buf + offset, (char*)& label, sizeof(label_t));
    	offset += sizeof(label_t);

    	return offset;
	}

//	void read_from_buf(char* buf){
//	}

	void write_unreadable(FILE* f){
    	fwrite((const void*)& srcId, sizeof(vertexid_t), 1, f);
    	fwrite((const void*)& dstId, sizeof(vertexid_t), 1, f);
		fwrite((const void*)& label, sizeof(label_t), 1, f);
	}

//	void load_unreadable(FILE* fp){
//
//	}


private:
	vertexid_t srcId;
	vertexid_t dstId;
	label_t label;
};


namespace std {
	template<>
	struct hash<Edge> {
		std::size_t operator()(const Edge& edge) const {
			//simple hash
			return std::hash<int>()(edge.get_hash());
		}
	};
}


#endif /* COMP_GRAPHSTORE_EDGE_H_ */
