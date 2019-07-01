/*
 * Logger.hpp
 *
 *  Created on: Mar 14, 2017
 *      Author: icuzzq
 */

//#include "../core/io_manager.hpp"
//#include "../common/RStreamCommon.hpp"
//#include "../core/type.hpp"

#ifndef SRC_PRINT_HPP_
#define SRC_PRINT_HPP_

#include "../common/CommonLibs.hpp"


class Logger{

private:
	static std::mutex lock_log;

public:
	static void print_thread_info_locked(const std::string & info, const int level){
		if(level == 0){
			return;
		}
		std::unique_lock<std::mutex> lock(lock_log);
		pid_t x = syscall(__NR_gettid);
		std::cout << "thread " << x << ": " << info;
	}

//	static void print_thread_info(const std::string & info){
//		pid_t x = syscall(__NR_gettid);
//		std::cout << "thread " << x << ": " << info;
//	}

	//template<typename T>
	//void printUpdateStream(int num_partitions, std::string fileName, Update_Stream in_stream){
	//	for(int i = 0; i < num_partitions; i++) {
	//		std::cout << "--------------------" + (fileName + "." + std::to_string(i) + ".update_stream_" + std::to_string(in_stream)) + "---------------------\n";
	//		int fd_update = open((fileName + "." + std::to_string(i) + ".update_stream_" + std::to_string(in_stream)).c_str(), O_RDONLY);
	//		assert(fd_update > 0 );
	//
	//		// get file size
	//		long update_file_size = io_manager::get_filesize(fd_update);
	//
	//		char * update_local_buf = new char[update_file_size];
	//		io_manager::read_from_file(fd_update, update_local_buf, update_file_size, 0);
	//
	//		// for each update
	//		for(size_t pos = 0; pos < update_file_size; pos += sizeof(T)) {
	//			// get an update
	//			T & update = *(T*)(update_local_buf + pos);
	//			std::cout << update << std::endl;
	//		}
	//	}
	//}

	static std::string generate_log_del(const std::string& i_string, int level){
		std::string idenote ("===================================================");
		std::string pdenote ("-----------------------------------");
		std::string space (" ");

		if(level == 1){
			return idenote + space + i_string + space + idenote;
		}
		if(level == 2){
			return pdenote + space + i_string + space + pdenote;
		}

		return nullptr;
	}

};




#endif /* SRC_PRINT_HPP_ */
