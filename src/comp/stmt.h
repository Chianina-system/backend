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
	friend std::ostream & operator<<(std::ostream & strm, const Stmt& stmt) {
		strm << "\nStmt:\t" << (int)stmt.t << ", " << stmt.getDst() << " <- " << stmt.getSrc() << ", " << stmt.getAdded() << endl;
		return strm;
	}


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

    bool isValidAdded() const {
    	return added != -1;
    }

    Stmt(TYPE t, vertexid_t src, vertexid_t dst, vertexid_t added = -1) : t(t), src(src), dst(dst), added(added) {}


    std::string toString(){
    	std::ostringstream out;
    	out << getTypeString(t) << ", " << getDst() << " <- " << getSrc() << ", " << getAdded();
    	return out.str();
    }

    std::string getTypeString(TYPE t){
    	switch(t){
    	case TYPE::Assign: return "Assign";
    	case TYPE::Load: return "Load";
    	case TYPE::Store: return "Store";
    	case TYPE::Alloca: return "Alloca";
    	default:
    		return nullptr;
    	}

    }

private:
	TYPE t;
	vertexid_t src;
	vertexid_t dst;
	vertexid_t added;

};




#endif /* COMP_STMT_H_ */
