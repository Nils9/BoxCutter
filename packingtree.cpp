#include "packingtree.h"

PackingTree::PackingTree(int h, int w)
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

std::vector<int> PackingTree::place(std::vector<int> size){
    std::vector<Node*> newSpace;
    std::vector<Node*> space;
    std::vector<int> alreadyStored;
    int area = size[1];
    int L = size[2];
    int l = size[3];
    //std::cout << "Espace libre avant placement : " << availableSpace.size() << std::endl;
    for(int i = 0; i < this->availableSpace.size(); i++){
        Node * currentNode = this->availableSpace[i];
        currentNode->getCoord();
        int currentArea = currentNode->getArea();
        int currentWidth = currentNode->getWidth();
        int currentHeight = currentNode->getHeight();
        if((area < currentArea) && (L < std::max(currentHeight, currentWidth)) && (l < std::min(currentHeight, currentWidth))){
            std::vector<int> newPlace = currentNode->place(size);
            alreadyStored = newPlace;
            //std::cout << "Coordonnées de la où je place le rect" << alreadyStored[1] << " " << alreadyStored[2] << " " << alreadyStored[3] << " " << alreadyStored[4] << std::endl;
            std::vector<Node*> newNodes = currentNode->splitNode(newPlace, newSpace);
            space = currentNode->modifySpace(this->availableSpace, newNodes);
            break;
        }
    }
    availableSpace = space;
    //std::cout << "Espace libre avant placement : " << availableSpace.size() << std::endl;
    return alreadyStored;
}
