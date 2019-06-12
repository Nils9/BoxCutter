#ifndef PACKINGTREE_H
#define PACKINGTREE_H
#include "node.h"
#include <vector>
#include <iostream>

class PackingTree
{
public:
    PackingTree(int h, int w);
    std::vector<int> place(std::vector<int> size);
    std::vector<Node*> getAvailableSpace();
    void setAvailableSpace(std::vector<Node*> space);

private:
    int width;
    int height;
    std::vector<Node*> availableSpace;
};

#endif // PACKINGTREE_H
