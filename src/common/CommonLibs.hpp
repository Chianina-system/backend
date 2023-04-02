

#ifndef SRC_COMMON_RSTREAMCOMMON_HPP_
#define SRC_COMMON_RSTREAMCOMMON_HPP_

#include <atomic>
#include <sys/syscall.h>
#include <iostream>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include <thread>
#include <fcntl.h>

#include <queue>
#include <mutex>
#include <condition_variable>

#include <fstream>
#include <cassert>
#include <vector>
#include <ostream>
#include <cmath>

#include <cstdint>
#include <cstdlib>
#include <time.h>
#include <signal.h>

#include <ctime>
#include <sstream>
#include <iomanip>

#include <functional>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <stdlib.h>

#include <exception>

//#include <boost/date_time/posix_time/posix_time_types.hpp>
//#include <boost/algorithm/string.hpp>
#include <algorithm>
#include <climits>
#include <memory>

#include <cstring>
#include <malloc.h>

#include <sys/time.h>
#include <sys/resource.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include <bits/stdc++.h>

using std::cout;
using std::endl;

//#include <boost/asio/io_service.hpp>
//#include <boost/bind.hpp>
//#include <boost/thread/thread.hpp>

//#include "../utility/Printer.hpp"

typedef uint32_t uint32;
typedef int32_t int32;

#if !(__APPLE__ & __MACH__)
typedef uint64_t uint64;
typedef int64_t int64;
#else
typedef size_t uint64;
typedef size_t int64;
#endif

typedef uint16_t uint16;
typedef int16_t int16;
typedef int8_t int8;
typedef uint8_t uint8;

typedef int vertexid_t;
typedef char label_t;
typedef int IR;
typedef int Line;
#define node_bound 20
#define CacheLineNum 512
#define CacheLineSize 64
#define CacheSetNum 4
#define CacheLinesPerSet 128

typedef int PEGraph_Pointer;
typedef int Partition;


#define GRAMMAR_STR_LEN 36

//#define readable 1

#define serialize_peg_mode 0
#define IS_PEGCOMPUTE_PARALLEL_ADD 0
#define IS_PEGCOMPUTE_PARALLEL_DELETE 0
// #define NUM_THREADS 16
#define NUM_THREADS 4
#define NUM_THREADS_CFGCOMPUTE 2



#define SUMMARY_MODE 0

//#define LEVEL_LOG_INFO 1
#define LEVEL_LOG_FUNCTION 1
#define LEVEL_LOG_PEG 1
#define LEVEL_LOG_GRAPHSTORE 1
#define LEVEL_LOG_CFGNODE 1
#define LEVEL_LOG_WORKLIST 1
#define LEVEL_LOG_MAIN 1

#endif /* SRC_COMMON_RSTREAMCOMMON_HPP_ */
