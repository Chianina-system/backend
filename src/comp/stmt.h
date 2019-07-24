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
	Alloca,
	Phi,
	Call,
	Return,
	Ret,
	Skip
};

class Stmt{
	friend std::ostream & operator<<(std::ostream & strm, const Stmt& stmt) {
//		strm << "\nStmt:\t" << (int)stmt.t << ", " << stmt.getDst() << " <- " << stmt.getSrc() << ", " << stmt.getAux() << endl;
		return strm;
	}


public:


	TYPE getType() const{
		return t;
	}

	vertexid_t getSrc() const{
		assert(this->length == 1);
		return src[0];
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



//    Stmt(TYPE t, vertexid_t src, vertexid_t dst, vertexid_t aux = -1) : t(t), src(src), dst(dst), auxiliary(aux) {}

//    Stmt(int l){
//    	this->length = l;
//    	this->src = new vertexid_t[this->length];
//    }

    ~Stmt(){
    	if(this->src){
    		delete[] src;
    	}
    }

    Stmt(std::stringstream& stream){
		std::string type, dst, src, aux;
		stream >> type;

//		std::cout << type << "," << dst << "," << src
//				<< "," << aux << "\n";

		if (type == "assign") {
			this->length = 1;
			this->src = new vertexid_t[this->length];

			this->t = TYPE::Assign;

			stream >> dst >> src;
			this->src[0] = atoi(src.c_str());
			this->dst = atoi(dst.c_str());
			this->auxiliary = -1;
		}
		else if (type == "load") {
			this->length = 1;
			this->src = new vertexid_t[this->length];

			this->t = TYPE::Load;

			stream >> dst >> src >> aux;
			this->src[0] = atoi(src.c_str());
			this->dst = atoi(dst.c_str());
			this->auxiliary = atoi(aux.c_str());
		}
		else if (type == "store") {
			this->length = 1;
			this->src = new vertexid_t[this->length];

			this->t = TYPE::Store;

			stream >> dst >> src >> aux;
			this->src[0] = atoi(src.c_str());
			this->dst = atoi(dst.c_str());
			this->auxiliary = atoi(aux.c_str());
		}
		else if (type == "alloca") {
			this->length = 1;
			this->src = new vertexid_t[this->length];

			this->t = TYPE::Alloca;

			stream >> dst >> src >> aux;
			this->src[0] = atoi(src.c_str());
			this->dst = atoi(dst.c_str());
			this->auxiliary = atoi(aux.c_str());
		}
		else if(type == "phi"){
			this->t = TYPE::Phi;

			stream >> dst;
			this->dst = atoi(dst.c_str());

			std::vector<std::string> vector_string;
			while(getline(stream, src, '\t')){
				vector_string.push_back(src);
//				cout << src << endl;
			}

			this->length = vector_string.size();
			this->src = new vertexid_t[this->length];
			int i = 0;
			for(auto& it: vector_string){
				this->src[i] = atoi(it.c_str());
				i++;
			}

			this->auxiliary = -1;

		}
		else if(type == "call"){
			this->t = TYPE::Call;
		}
		else if(type == "return"){
			this->t = TYPE::Return;
		}
		else if(type == "ret"){
			this->t = TYPE::Ret;
		}
		else if(type == "block"){
			this->t = TYPE::Skip;
		}
		else{
			cout << "wrong stmt type!!!" << endl;
			exit(EXIT_FAILURE);
		}



//		if(aux == ""){
//			this->auxiliary = -1;
//		}
//		else{
//			this->auxiliary = atoi(aux.c_str());
//		}
//
//		std::cout << (int)this->t << "," << this->dst << "," << this->src
//						<< "," << this->auxiliary << "\n";

    }


    std::string toString(){
    	std::ostringstream out;
    	out << getTypeString(t) << ", " << getDst() << " <- " << getSrc() << ", " << getAux() << ", ";
    	out << getTypeString(t) << ", " << getFlag();
    	return out.str();
    }

    std::string getTypeString(TYPE t){
    	switch(t){
    	case TYPE::Assign: return "Assign";
    	case TYPE::Load: return "Load";
    	case TYPE::Store: return "Store";
    	case TYPE::Alloca: return "Alloca";
    	case TYPE::Phi: return "Phi";
    	case TYPE::Call: return "Call";
    	case TYPE::Return: return "Return";
    	case TYPE::Ret: return "Ret";
    	case TYPE::Skip: return "Skip";
    	default:
    		return nullptr;
    	}

    }

	int getLength() const {
		return length;
	}

	vertexid_t* getSrcs() const {
		return src;
	}

	bool getFlag()  const {
		return flag;
	}

	void setFlag() {
		flag = true;
	}

private:
    int length;
	vertexid_t* src = nullptr;
	vertexid_t dst;
	vertexid_t auxiliary;
	TYPE t;
	bool flag = false;

};




#endif /* COMP_STMT_H_ */
