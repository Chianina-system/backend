/*
 * edge.h
 *
 *  Created on: Sep 21, 2019
 *      Author: dell
 */

#ifndef COMP_GRAPHSTORE_EDGE_H_
#define COMP_GRAPHSTORE_EDGE_H_

#include "../../common/CommonLibs.hpp"

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

	inline vertexid_t getSrcId() const {
		return srcId;
	}

	int get_hash() const{
		return ((srcId << 1) + (dstId >> 1) + label);
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
