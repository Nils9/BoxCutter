#ifndef NODE_H
#define NODE_H

#include <vector>
#include <iostream>

class Node
{
public:
    Node(int h, int w, int xmin, int ymin, int xmax, int ymax);
    int getWidth();
    int getHeight();
    int getArea();
    void getCoord();
    std::vector<int> getCoordinates();
    std::vector<int> place(std::vector<int> size);
    std::vector<Node*> splitNode(std::vector<int> newPlace, std::vector<Node*> newSpace);
    std::vector<Node*> modifySpace(std::vector<Node*> space, std::vector<Node*> newNodes);

private:
    int width;
    int height;
    int area;
    int xmin;
    int ymin;
    int xmax;
    int ymax;
};

#endif // NODE_H
