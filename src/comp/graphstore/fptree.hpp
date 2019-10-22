/*
 * fptree.hpp
 *
 *  Created on: Oct 21, 2019
 *      Author: nju-seg-hsy
 */

#ifndef COMP_GRAPHSTORE_FPTREE_HPP_
#define COMP_GRAPHSTORE_FPTREE_HPP_


//#ifndef FPTREE_HPP
//#define FPTREE_HPP

#include <cstdint>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <utility>


using Item = int; //m
using Transaction = std::vector<Item>;
using TransformedPrefixPath = std::pair<std::vector<Item>, uint64_t>;
using Pattern = std::pair<std::set<Item>, uint64_t>;

// struct comparePattern
// {
// 	bool operator() (const Pattern &p1, const Pattern &p2)
// 	{
// 		if (p1.second==p2.second)
//             return (p1.first).size()<(p2.first).size();
//         return p1.second>p2.second;
// 	}
// };

struct FPNode {
    const Item item;
    uint64_t frequency;
    std::shared_ptr<FPNode> node_link;
    std::weak_ptr<FPNode> parent;
    std::vector<std::shared_ptr<FPNode>> children;

    FPNode(const Item&, const std::shared_ptr<FPNode>&);
};

struct FPTree {
    std::shared_ptr<FPNode> root;
    std::map<Item, std::shared_ptr<FPNode>> header_table;
    uint64_t minimum_support_threshold;

    FPTree(const std::vector<Transaction>&, uint64_t);

    bool empty() const;
};


std::set<Pattern> fptree_growth(const FPTree&);

//#endif  // FPTREE_HPP



#endif /* COMP_GRAPHSTORE_FPTREE_HPP_ */
