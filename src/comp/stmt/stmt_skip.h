/*
 * stmt_skip.h
 *
 *  Created on: Jul 28, 2019
 *      Author: dell
 */

#ifndef COMP_STMT_SKIP_H_
#define COMP_STMT_SKIP_H_

#include "stmt.h"

class SkipStmt: public Stmt {

public:

	SkipStmt(){
		this->t = TYPE::Skip;
	}




private:
    void toString_sub(std::ostringstream& strm) const {
    	strm << "skip";
    }


};




#endif /* COMP_STMT_SKIP_H_ */
