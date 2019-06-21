#ifndef PACKINGTREE_H
#define PACKINGTREE_H
#include "node.h"
#include <vector>
#include <iostream>

class PackingTree
{
public:
    PackingTree(double h, double w);
    std::vector<double> place(std::vector<double> size);
    std::vector<Node*> getAvailableSpace();
    void setAvailableSpace(std::vector<Node*> space);

private:
    double width;
    double height;
    std::vector<Node*> availableSpace;
};

#endif // PACKINGTREE_H
