///*
// * priority_set.h
// *
// *  Created on: Jul 27, 2019
// *      Author: dell
// */
//
//#ifndef COMP_PRIORITY_SET_H_
//#define COMP_PRIORITY_SET_H_
//
//
//#include "partition_priority.h"
//
//
//class partition_info {
//
//	friend std::ostream & operator<<(std::ostream & strm, const partition_info& partition_info) {
//		strm << "<pid=" << partition_info.partition_id << ", score=" << partition_info.score << ">";
//		return strm;
//	}
//
//public:
//	~partition_info(){}
//
////	partition_info(unsigned int id, int s){
////		this->partition_id = id;
////		this->score = s;
////	}
//
//	partition_info(unsigned int id, int s = 0){
//		this->partition_id = id;
//		this->score = s;
//	}
//
//	//operator for map
//	bool operator==(const partition_info& other) const {
//		return this->partition_id == other.partition_id;
//	}
//
//	void increase_score(int s){
//		this->score += s;
//	}
//
//
////private:
//	unsigned int partition_id;
//	int score;
//
//};
//
//namespace std {
//	template<>
//	struct hash<partition_info> {
//		std::size_t operator()(const partition_info& pinfo) const {
//			//simple hash
//			return std::hash<int>()(pinfo.partition_id);
//		}
//	};
//}
//
//struct partition_compare {
//    bool operator() (const partition_info& lhs, const partition_info& rhs) const {
//        return lhs.score == rhs.score ? lhs.partition_id < rhs.partition_id : lhs.score < rhs.score;
//    }
//};
//
//
//class Priority_set: public Priority_partition {
//
//public:
//	~Priority_set(){}
//
//	bool schedule(Partition& part){
//		if(priority_set.empty()){
//			return false;
//		}
//		else{
//			auto first = priority_set.begin();
//			part = first->partition_id;
//			priority_set.erase(first);
//			return true;
//		}
//	}
//
//	void update_priority(Partition part, int size){
//		partition_info pinfo(part, size);
//		auto it = priority_set.find(pinfo);
//		if(it != priority_set.end()){
//			int old_score = it->score;
//			priority_set.erase(it);
//			pinfo.increase_score(old_score);
//			priority_set.insert(pinfo);
//		}
//		else{
//			priority_set.insert(pinfo);
//		}
//	}
//
//	void printOutPriorityInfo(){
//		cout << "\nPriority set: {";
//		for (auto &it : this->priority_set) {
//			cout << it << "\t";
//		}
//		cout << "}\n" << endl;
//	}
//
//
//private:
//	std::set<partition_info, partition_compare> priority_set;
//
//};
//
//
//#endif /* COMP_PRIORITY_SET_H_ */
