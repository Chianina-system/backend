/*
 * stmt_callfptr.h
 *
 *  Created on: April 20, 2020
 *      Author: dell
 */

#ifndef STMT_CALLFPTR_H_
#define STMT_CALLFPTR_H_

#include "stmt.h"

class CallfptrStmt: public Stmt {

public:
  CallfptrStmt(){
    this->t = TYPE::Callfptr;
    this->dst = -1;
    this->auxiliary = -1;

    this->length = 0;
    this->args = NULL;
    this->ret = -1;
  }

  ~CallfptrStmt(){
      if(this->args){
        delete[] args;
      }
  }

  CallfptrStmt(std::stringstream& stream){
    this->t = TYPE::Callfptr;

    std::string str_dst, aux;
    stream >> str_dst;
    this->dst = atoi(str_dst.c_str());

    std::unordered_set<std::string> set_string;

    if(stream >> aux){
      if(aux.at(0) == 'a'){
        set_string.insert(aux.substr(2));
        this->auxiliary = this->dst ;
      }
      else if(aux.at(0) == 'r'){
        ret = atoi(aux.substr(2).c_str());
        this->auxiliary = this->dst ;
      }
      else if(aux.at(0) >= '0' && aux.at(0) <= '9'){
        this->auxiliary = atoi(aux.c_str());
      }
      else {
        cout << "wrong arg type!!!" << endl;
        exit(EXIT_FAILURE);
      }
    }

    std::string arg;
    while(stream >> arg){
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


  vertexid_t getDst() const{
    return dst;
  }

    vertexid_t getAux() const{
        return auxiliary;
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
      return sizeof(vertexid_t) * 2 + sizeof(vertexid_t) + sizeof(vertexid_t) * length;;
    }

    void write_to_buf(Buffer& buf) {
      memcpy(buf.getData() + buf.getSize(), (char*)& dst, sizeof(vertexid_t));
      buf.add_size_by(sizeof(vertexid_t));
      memcpy(buf.getData() + buf.getSize(), (char*)& auxiliary, sizeof(vertexid_t));
      buf.add_size_by(sizeof(vertexid_t));

      memcpy(buf.getData() + buf.getSize(), (char*)& ret, sizeof(vertexid_t));
      buf.add_size_by(sizeof(vertexid_t));
      memcpy(buf.getData() + buf.getSize(), args, sizeof(vertexid_t) * length);
      buf.add_size_by(sizeof(vertexid_t) * length);
    }

    void read_from_buf(char* buf, size_t offset, size_t bufsize){
      dst = *((vertexid_t*)(buf + offset));
      offset += sizeof(vertexid_t);
      auxiliary = *((vertexid_t*)(buf + offset));
      offset += sizeof(vertexid_t);
//      assert(offset == bufsize);

      ret = *((vertexid_t*)(buf + offset));
      offset += sizeof(vertexid_t);
      length = (bufsize - offset) / sizeof(vertexid_t);
      this->args = new vertexid_t[length];
      memcpy(args, buf + offset, sizeof(vertexid_t) * length);
    }



private:

  vertexid_t dst = -1;
  vertexid_t auxiliary = -1;

    int length = 0;
  vertexid_t* args = NULL;
  vertexid_t ret = -1;

    void toString_sub(std::ostringstream& strm) const {
      strm << "callfptr" << ", " << getDst() << ", " << getAux() << ", " << getRet() << " <- [";
      for(int i = 0; i < length; i++){
        strm << args[i] << ", ";
      }
      strm << "]";
    }

};



#endif /* STMT_CALLFPTR_H_ */
