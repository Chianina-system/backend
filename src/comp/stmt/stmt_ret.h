/*
 * stmt_ret.h
 *
 *  Created on: Jul 28, 2019
 *      Author: dell
 */

#ifndef COMP_STMT_RET_H_
#define COMP_STMT_RET_H_

#include "stmt.h"

class RetStmt: public Stmt {

public:

	RetStmt(){
		this->t = TYPE::Ret;
	}




private:
    void toString_sub(std::ostringstream& strm) const {
    	strm << "ret";
    }


};



#endif /* COMP_STMT_RET_H_ */
