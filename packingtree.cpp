#include "packingtree.h"

PackingTree::PackingTree(double h, double w)
{
    this->height = h;
    this->width = w;
    Node * node = new Node(h, w, 0, 0, h, w);
    availableSpace.push_back(node);
}

std::vector<Node*> PackingTree::getAvailableSpace(){
    return this->availableSpace;
}

void PackingTree::setAvailableSpace(std::vector<Node*> space){
    this->availableSpace = space;
}

std::vector<double> PackingTree::place(std::vector<double> size){
    std::vector<Node*> newSpace;
    std::vector<Node*> space;
    std::vector<double> alreadyStored;
    double area = size[1];
    double L = size[2];
    double l = size[3];
    for(int i = 0; i < this->availableSpace.size(); i++){
        Node * currentNode = this->availableSpace[i];
        double currentArea = currentNode->getArea();
        double currentWidth = currentNode->getWidth();
        double currentHeight = currentNode->getHeight();
        if((area < currentArea) && (L < std::max(currentHeight, currentWidth)) && (l < std::min(currentHeight, currentWidth))){
            std::vector<double> newPlace = currentNode->place(size);
            alreadyStored = newPlace;
            std::vector<Node*> newNodes = currentNode->splitNode(newPlace, newSpace);
            space = currentNode->modifySpace(this->availableSpace, newNodes);
            break;
        }
    }
    availableSpace = space;
    return alreadyStored;
}
