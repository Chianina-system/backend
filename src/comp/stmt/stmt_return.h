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
	}




private:
    void toString_sub(std::ostringstream& strm) const {
    	strm << "return";
    }


};




#endif /* COMP_STMT_RETURN_H_ */
