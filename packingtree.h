#ifndef PACKINGTREE_H
#define PACKINGTREE_H
#include "node.h"
#include <vector>

class PackingTree
{
public:
    PackingTree();

private:
    std::vector<Node> availableSpace;
    std::vector<std::vector<int>> alreadyStored;
};

#endif // PACKINGTREE_H
