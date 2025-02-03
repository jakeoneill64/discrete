#ifndef DISCRETE_KDTREE_H
#define DISCRETE_KDTREE_H

#include "vector.h"
#include "fixed_point.h"
#include <stdexcept>
#include <concepts>
#include <memory>

//NOLINTBEGIN
template <typename Node>
class KDNodeWrapper{
public:
    template <typename ReturnType>
    const ReturnType& getDimension(const Node& node, unsigned int index);
    unsigned int dimensions;
};

template<>
class KDNodeWrapper<vec3>{
public:
    const fixed64_mid& getDimension(const vec3& node, unsigned int index){
        switch (index) {
            case 0: return node.i;
            case 1: return node.j;
            case 2: return node.k;
            default: throw std::out_of_range("Index out of range");
        }
    }
    unsigned int dimensions = 3;
};
//NOLINTEND

template <typename Node, typename... Nodes>
concept AllSame = (std::same_as<Nodes, Node> && ...);

// TODO future consideration might be to implement a weak KDTree where the underlying data is not
// owned by the tree. std::weak_ptr. Not sure about the necessity for this for now.
/** Self-balancing KD-tree for efficient multi-dimensional search.
 *
 * @tparam Node
 */
template <typename Node>
class KDTree {
public:

    std::shared_ptr<Node> insert(const Node& node){

    }

    bool deleteItem(const Node& node){

    }

    template <typename... Nodes>
    requires AllSame<Node, Nodes...>
    std::vector<Node> deleteRange(const Nodes&...){

    }

    template <typename... Nodes>
    requires AllSame<Node, Nodes...>
    std::vector<Node> searchRange(Nodes...){

    }

    std::optional<std::shared_ptr<Node>> searchItem(const Node& node){

    }

};

#endif //DISCRETE_KDTREE_H
