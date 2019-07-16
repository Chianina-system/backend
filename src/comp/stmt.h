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
		strm << "\nStmt:\t" << (int)stmt.t << ", " << stmt.getDst() << " <- " << stmt.getSrc() << ", " << stmt.getAux() << endl;
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

    vertexid_t getAux() const{
        return auxiliary;
    }

    bool isValidAux() const {
    	return auxiliary != -1;
    }

    Stmt(TYPE t, vertexid_t src, vertexid_t dst, vertexid_t aux = -1) : t(t), src(src), dst(dst), auxiliary(aux) {}

    Stmt(std::stringstream& stream){
		std::string type, dst, src, aux;
		stream >> type >> dst >> src >> aux;

		std::cout << type << "," << dst << "," << src
				<< "," << aux << "\n";

		if (type == "assign") {
			this->t = TYPE::Assign;
		}
		if (type == "load") {
			this->t = TYPE::Load;
		}
		if (type == "store") {
			this->t = TYPE::Store;
		}
		if (type == "alloca") {
			this->t = TYPE::Alloca;
		}

		this->src = atoi(src.c_str());
		this->dst = atoi(dst.c_str());
		if(aux == ""){
			this->auxiliary = -1;
		}
		else{
			this->auxiliary = atoi(aux.c_str());
		}

		std::cout << (int)this->t << "," << this->dst << "," << this->src
						<< "," << this->auxiliary << "\n";

    }


    std::string toString(){
    	std::ostringstream out;
    	out << getTypeString(t) << ", " << getDst() << " <- " << getSrc() << ", " << getAux();
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
	vertexid_t auxiliary;

};




#endif /* COMP_STMT_H_ */
