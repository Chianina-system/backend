/*
 * Stmt.h
 *
 *  Created on: May 20, 2019
 *      Author: zqzuo
 */

#ifndef COMP_STMT_H_
#define COMP_STMT_H_
#include "../common/CommonLibs.hpp"

enum class TYPE {
	Assign,
	Load,
	Store,
	Alloca
};

class Stmt{

public:


	TYPE getType() const{
		return t;
	}

	vertexid_t getSrc() const{
		return src;
	}

	vertexid_t getDst() const{
		return dst;
	}

    vertexid_t getAdded() const{
        return added;
    }

    Stmt(TYPE t, vertexid_t src, vertexid_t dst, vertexid_t added = -1) : t(t), src(src), dst(dst), added(added) {}

private:
	TYPE t;
	vertexid_t src;
	vertexid_t dst;
	vertexid_t added;

};




#endif /* COMP_STMT_H_ */
