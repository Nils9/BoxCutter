#ifndef PROJECTMESH_H
#define PROJECTMESH_H

#include <vector>
#include "point3.h"
#include "point2.h"



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
    unsigned int & operator [] (unsigned int c) { return corners[c]; }
    unsigned int operator [] (unsigned int c) const { return corners[c]; }
    unsigned int size() const { return 3 ; }
};
struct Mesh{
    std::vector< Vertex > vertices;
    std::vector< Triangle > triangles;
    std::vector< uvCoord > textcoords;
    std::vector< Triangle > triangles_text;
};



#endif // PROJECTMESH_H
