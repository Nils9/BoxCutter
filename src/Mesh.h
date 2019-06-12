#ifndef PROJECTMESH_H
#define PROJECTMESH_H

#include <vector>
#include "point3.h"
#include "point2.h"
#include "QPoint"



struct Vertex{
    point3d p;
    Vertex() {}
    Vertex(double x , double y , double z) : p(x,y,z) {}
    double & operator [] (unsigned int c) { return p[c]; }
    double operator [] (unsigned int c) const { return p[c]; }
};
struct uvCoord{
    point2d uv;
    uvCoord(double x , double y ) : uv(x,y) {}
    double & operator [] (unsigned int c) { return uv[c]; }
    double operator [] (unsigned int c) const { return uv[c]; }
};
struct Triangle{
    unsigned int corners[3];
    //Triangle(unsigned int i, unsigned int j, unsigned int k){corners[0] = i; corners[1] = j; corners[2] = k;}
    unsigned int & operator [] (unsigned int c) { return corners[c]; }
    unsigned int operator [] (unsigned int c) const { return corners[c]; }
    unsigned int size() const { return 3 ; }
};
struct Mesh{
    std::vector< Vertex > vertices;
    std::vector< Triangle > triangles;
    std::vector< uvCoord > textcoords;
    std::vector< Triangle > triangles_text;
    std::vector<std::vector<int>> neighbours;
    std::vector<std::vector<int>> charts;
    std::vector<std::vector<Triangle>> chartsTriangles;
    std::vector<std::vector<double>> chartsBoundingBoxes;
    std::vector<int> chartOfVertex;
    void updateNeighbours(){
        for(int k = 0; k<textcoords.size(); k++){
            std::vector<int> vec = std::vector<int>();
            neighbours.push_back(vec);
        }
        for(int i = 0; i<triangles_text.size();i++){
            int corner0 = triangles_text[i][0];
            int corner1 = triangles_text[i][1];
            int corner2 = triangles_text[i][2];

            neighbours[corner0].push_back(corner1);
            neighbours[corner1].push_back(corner2);
            neighbours[corner2].push_back(corner0);
        }
    }
    void updateTextTriangles(){
        std::vector<Triangle> newTrianglesText = std::vector<Triangle>();
        for(int k = 0; k<chartsTriangles.size(); k++){
            for(int i = 0; i<chartsTriangles[k].size(); i++){
                newTrianglesText.push_back(chartsTriangles[k][i]);
            }
        }
        std::cout<<"nombre de triangles dans newTrianglesText à la fin de updateTextTriangles: "<<newTrianglesText.size()<<std::endl;
        triangles_text = newTrianglesText;
    }
    void invertXY(int chartIndex){
        for(int i = 0; i<charts[chartIndex].size(); i++){
            textcoords[charts[chartIndex][i]] = uvCoord(textcoords[charts[chartIndex][i]][1],textcoords[charts[chartIndex][i]][0]);
        }
    }
    void moveVertices(int chartIndex, double deltaX, double deltaY){
        std::cout<<"chartIndex: "<<chartIndex<<std::endl;
        std::cout<<"deltaX: "<<deltaX<<", deltaY: "<<deltaY<<std::endl;
        std::cout<<"charts[chartIndex].size(): "<<charts[chartIndex].size()<<std::endl;
        for(int i = 0; i<charts[chartIndex].size(); i++){
            textcoords[charts[chartIndex][i]] = uvCoord(textcoords[charts[chartIndex][i]][0]+deltaX,textcoords[charts[chartIndex][i]][1]+deltaY);
        }
    }
};



#endif // PROJECTMESH_H
