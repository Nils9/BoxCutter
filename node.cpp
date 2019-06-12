#include "node.h"

Node::Node(int h, int w, int xmin, int ymin, int xmax, int ymax)
{
    this->height = h;
    this->width = w;
    this->xmin = xmin;
    this->ymin = ymin;
    this->xmax = xmax;
    this->ymax = ymax;
    this->area = h*w;
}

int Node::getArea(){
    return this->area;
}

int Node::getHeight(){
    return this->height;
}

int Node::getWidth(){
    return this->width;
}

std::vector<int> Node::getCoordinates(){

    std::vector<int> coordinates;
    coordinates.push_back(this->xmin);
    coordinates.push_back(this->ymin);
    coordinates.push_back(this->xmax);
    coordinates.push_back(this->ymax);

    return coordinates;
}

//Place un rectangle dans un noeud en partant du haut gauche de façon à ce que la croissance verticale soit minimale
std::vector<int> Node::place(std::vector<int> size){
    std::vector<int> newPlace;
    newPlace.push_back(size[0]);
    newPlace.push_back(this->xmin);
    newPlace.push_back(this->ymin);
    int L = size[2];
    int l = size[3];

    if(L < this->width){
        newPlace.push_back(this->xmin + L);
        newPlace.push_back(this->ymin + l);
    }
    else{
        newPlace.push_back(this->xmin + l);
        newPlace.push_back(this->ymin + L);
    }

    return newPlace;
}

std::vector<Node*> Node::splitNode(std::vector<int> newPlace, std::vector<Node*> newSpace){
    int x0 = newPlace[1];
    int y0 = newPlace[2];
    int x1 = newPlace[3];
    int y1 = newPlace[4];
    std::vector<Node*> newNodes;
    //std::cout << "coordonnées du noeud splité" << std::endl;
    this->getCoord();
    if(x1 < this->xmax){ // cas où il existe un noeud fils à droite
        int newWidth = this->xmax - x1 - 1;
        int newHeight = this->ymax - y0;
        int newArea = newWidth * newHeight;
        Node * node1 = new Node(newHeight, newWidth, x1 + 1, y0, this->xmax, this->ymax);
        //std::cout << "coordonnées du noeud rajouté en dessous" << std::endl;
        node1->getCoord();
        newNodes.push_back(node1);

        if(y1 < this->ymax){ //cas où il existe aussi un noeud fils bas
            int newWidth2 = x1 - x0;
            int newHeight2 = this->ymax - y1 - 1;
            int newArea2 = newWidth2 * newHeight2;
            Node * node2 = new Node(newWidth2, newHeight2, x0, y1 + 1, x1, this->ymax);
            //std::cout << "coordonnées du noeud rajouté en dessous" << std::endl;
            node2->getCoord();
            newNodes.push_back(node2);
        }
    }
    return newNodes;
}

std::vector<Node*> Node::modifySpace(std::vector<Node*> space, std::vector<Node*> newNodes){
    //placer les nouveaux noeuds
    for(int i = 0; i < newNodes.size(); i++){
        Node * node = newNodes[i];
        int newArea = node->getArea();
        int size = space.size();
        Node * firstNode = space[0];
        Node * lastNode = space[size - 1];
        if(newArea < firstNode->getArea()){
            space.insert(space.begin(), node);

        }
        else if(newArea > lastNode->getArea()){
            space.push_back(node);
        }
        else{
            for(int j = 0; j < size - 1; j++){
                Node * currentNode = space[j];
                int currentArea = currentNode->getArea();
                if(newArea > currentArea){
                    space.insert(space.begin() + j + 1, node);
                    break;
                }
            }
        }
    }

    //effacer le père
    for(int i = 0; i < space.size(); i++){
        Node * currentNode = space[i];
        if(currentNode == this){
            space.erase(space.begin() + i);
        }
    }
    return space;
}

void Node::getCoord(){
    //std::cout << "Coordonnées du noeud : " << xmin << " " << ymin << " " << xmax << " " << ymax << std::endl;
}
