/*
 * io_manager.hpp
 *
 *  Created on: Mar 3, 2017
 *      Author: kai
 */

#ifndef CORE_IO_MANAGER_HPP_
#define CORE_IO_MANAGER_HPP_

#include "../common/CommonLibs.hpp"

using namespace std;


	class IO_Manager {

	template <typename T> const char* getFormattedString();
	template<> const char* getFormattedString<double>(){return "%g\n";}
	template<> const char* getFormattedString<float>(){return "%g\n";}
	template<> const char* getFormattedString<int>(){return "%d\n";}
	template<> const char* getFormattedString<size_t>(){return "%lu\n";}

	constexpr size_t LEN = 32;


	public:
		static size_t get_filesize(int fd) {
			if(fd < 0) {
				std::cout << "Could not open file " << fd << std::endl;
				assert(false);
			}

			off_t size = lseek(fd, 0, SEEK_END);
			return size;
		}

		// how to read efficiently?
//		static void read_from_file(int fd, char * buf, size_t fsize) {
//			size_t n_read = 0;
//			while(n_read < fsize) {
//				ssize_t n_bytes = pread(fd, buf, fsize - n_read, n_read);
//				if(n_bytes == ssize_t(-1)) {
//					std::cout << "Read error ! " << std::endl;
//					std::cout << strerror(errno) << std::endl;
//					assert(false);
//				}
////				std::cout << n_bytes << std::endl;
//				assert(n_bytes > 0);
//				buf += n_bytes;
//				n_read += n_bytes;
//			}
//
//			assert(n_read <= fsize);
//		}

		static void read_from_file(int fd, char * buf, size_t fsize, size_t offset) {
			size_t n_read = 0;
			while(n_read < fsize) {
				ssize_t n_bytes = pread(fd, buf, fsize - n_read, offset + n_read);
				if(n_bytes == ssize_t(-1)) {
					std::cout << "Read error ! " << std::endl;
					std::cout << strerror(errno) << std::endl;
					assert(false);
				}
//				std::cout << n_bytes << std::endl;
				assert(n_bytes > 0);
				buf += n_bytes;
				n_read += n_bytes;
			}

			assert(n_read <= fsize);
		}

		// how to write efficiently
		static void write_to_file(int fd, char * buf, size_t fsize) {
			size_t n_write = 0;
			assert(fd > 0);

			while(n_write < fsize) {
				ssize_t n_bytes = pwrite(fd, buf, fsize - n_write, n_write);
				if(n_bytes == ssize_t(-1)) {
					std::cout << "Write error! " << std::endl;
					std::cout << strerror(errno) << std::endl;
					assert(false);
				}
				assert(n_bytes > 0);
				buf += n_bytes;
				n_write += n_bytes;
			}
		}

		static void write_to_file(const string& file_name_str, char * buf, size_t fsize){
			const char * file_name = file_name_str.c_str();
			int perms = O_WRONLY | O_APPEND;
			int fd = open(file_name, perms, S_IRWXU);
			if(fd < 0){
				fd = creat(file_name, S_IRWXU);
			}
			// flush buffer to update out stream
			write_to_file(file_name, buf, fsize);
			close(fd);
		}

		static void append_to_file(int fd, char * buf, size_t fsize) {
			assert(fd > 0);

			off_t offset = lseek(fd, 0, SEEK_END);
			size_t n_write = 0;
			assert(fd > 0);

			while(n_write < fsize) {
				ssize_t n_bytes = pwrite(fd, buf, fsize - n_write, n_write + offset);
				if(n_bytes == ssize_t(-1)) {
					std::cout << "Write error! " << std::endl;
					std::cout << strerror(errno) << std::endl;
					assert(false);
				}
				assert(n_bytes > 0);
				buf += n_bytes;
				n_write += n_bytes;
			}
		}


	    template <typename Iterator>
	    static auto toStringStream(Iterator begin, Iterator end, std::stringstream &buffer) {
	        char aLine[LEN];
	        const char* fmtStr = getFormattedString<typename std::iterator_traits<Iterator>::value_type>();
	        std::for_each(begin, end, [&buffer, &aLine, &fmtStr](const auto value) {
	            sprintf(aLine, fmtStr, value);
	            buffer << aLine;
	        });
	    }

	    // Use sprintf
	    template <typename Iterator>
	    static void sprintf_approach(Iterator begin, Iterator end, const std::string &fileName) {
	        std::stringstream buffer;
	        toStringStream(begin, end, buffer);
	        std::ofstream fout(fileName);
	        fout << buffer.str();
	        fout.close();
	    }

	};




#endif /* CORE_IO_MANAGER_HPP_ */
