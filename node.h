#ifndef NODE_H
#define NODE_H

#include <vector>
#include <iostream>

class Node
{
public:
    Node(double h, double w, double xmin, double ymin, double xmax, double ymax);
    double getWidth();
    double getHeight();
    double getArea();
    std::vector<double> getCoordinates();
    std::vector<double> place(std::vector<double> size);
    std::vector<Node*> splitNode(std::vector<double> newPlace, std::vector<Node*> newSpace);
    std::vector<Node*> modifySpace(std::vector<Node*> space, std::vector<Node*> newNodes);

private:
    double width;
    double height;
    double area;
    double xmin;
    double ymin;
    double xmax;
    double ymax;
};

#endif // NODE_H
