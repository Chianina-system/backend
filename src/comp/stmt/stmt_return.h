/*
 * stmt_return.h
 *
 *  Created on: Jul 28, 2019
 *      Author: dell
 */

#ifndef COMP_STMT_RETURN_H_
#define COMP_STMT_RETURN_H_

#include "stmt.h"

class ReturnStmt: public Stmt {

public:

	ReturnStmt(){
		this->t = TYPE::Return;

		this->length = 0;
		this->args = NULL;
		this->ret = -1;
	}

	~ReturnStmt(){
    	if(this->args){
    		delete[] args;
    	}
	}

	ReturnStmt(std::stringstream& stream){
		this->t = TYPE::Call;

		std::string arg;
		std::unordered_set<std::string> set_string;
		while(getline(stream, arg, '\t')){
			if(arg.at(0) == 'a'){
				set_string.insert(arg.substr(2));
			}
			else if(arg.at(0) == 'r'){
				this->ret = atoi(arg.substr(2).c_str());
			}
			else {
				cout << "wrong arg type!!!" << endl;
				exit(EXIT_FAILURE);
			}
		}

		this->length = set_string.size();
		this->args = new vertexid_t[this->length];
		int i = 0;
		for(auto& it: set_string){
			this->args[i] = atoi(it.c_str());
			i++;
		}
	}

    vertexid_t getRet() const {
    	return ret;
    }

	int getLength() const {
		return length;
	}

	vertexid_t* getArgs() const {
		return args;
	}


    size_t get_size_bytes() const {
    	return sizeof(vertexid_t) + sizeof(vertexid_t) * length;
    }

    void write_to_buf(Buffer& buf) {
    	memcpy(buf.getData() + buf.getSize(), (char*)& ret, sizeof(vertexid_t));
    	buf.add_size_by(sizeof(vertexid_t));
    	memcpy(buf.getData() + buf.getSize(), args, sizeof(vertexid_t) * length);
    	buf.add_size_by(sizeof(vertexid_t) * length);
    }

    void read_from_buf(char* buf, size_t offset, size_t bufsize){
    	ret = *((vertexid_t*)(buf + offset));
    	offset += sizeof(vertexid_t);
    	length = (bufsize - offset) / sizeof(vertexid_t);
    	this->args = new vertexid_t[length];
    	memcpy(args, buf + offset, sizeof(vertexid_t) * length);
    }


private:

    int length = 0;
	vertexid_t* args = NULL;
	vertexid_t ret = -1;


    void toString_sub(std::ostringstream& strm) const {
    	strm << "return";
    }


};




#endif /* COMP_STMT_RETURN_H_ */
