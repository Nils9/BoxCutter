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
    std::vector<std::vector<double>> getBoundingBoxes(){
        std::vector<std::vector<double>> boundingBoxes = std::vector<std::vector<double>>();
        std::cout<<"chartsTriangles.size(): "<<chartsTriangles.size()<<std::endl;
        std::cout<<"charts.size(): "<<charts.size()<<std::endl;
        for(int i = 0; i<chartsTriangles.size(); i++){
            std::cout<<"dans mesh.getBoundingBoxes() "<<i<<" ème chart"<<std::endl;
            std::cout<<"charts[i].size(): "<<charts[i].size()<<std::endl;
            std::cout<<"chartsTriangles[i].size(): "<<chartsTriangles[i].size()<<std::endl;
            if(charts[i].size() != 0){
                std::vector<double> bbox = std::vector<double>();
                bbox.push_back(textcoords[charts[i][0]][0]);
                bbox.push_back(textcoords[charts[i][0]][1]);
                bbox.push_back(textcoords[charts[i][0]][0]);
                bbox.push_back(textcoords[charts[i][0]][1]);
                std::cout<<"chartsTriangles[i].size()"<<chartsTriangles[i].size()<<std::endl;
                for(int j = 0; j<chartsTriangles[i].size(); j++){

                    int i0 = chartsTriangles[i][j][0];
                    int i1 = chartsTriangles[i][j][0];
                    int i2 = chartsTriangles[i][j][0];

                    bbox[0] = std::min(bbox[0],std::min(textcoords[i0][0],std::min(textcoords[i1][0],textcoords[i2][0])));
                    bbox[1] = std::min(bbox[1],std::min(textcoords[i0][1],std::min(textcoords[i1][1],textcoords[i2][1])));
                    bbox[2] = std::max(bbox[2],std::max(textcoords[i0][0],std::max(textcoords[i1][0],textcoords[i2][0])));
                    bbox[3] = std::max(bbox[3],std::max(textcoords[i0][1],std::max(textcoords[i1][1],textcoords[i2][1])));
                }
                boundingBoxes.push_back(bbox);
            }
            else{
                std::vector<double> bbox = std::vector<double>();
                bbox.push_back(0.);
                bbox.push_back(0.);
                bbox.push_back(0.);
                bbox.push_back(0.);
                boundingBoxes.push_back(bbox);
            }

        }
        return boundingBoxes;
    }

    void cutMeshText(int wbuffer, int hbuffer, int directionOfCut, int cutPositionPixel){
        std::cout<<"on rentre dans cutMeshText"<<std::endl;
        double cutPosition;
        switch(directionOfCut){
            case 0: cutPosition = (double) cutPositionPixel/(double)wbuffer;
                    break;
            case 1: cutPosition = (double)cutPositionPixel/(double)hbuffer;
                    break;
        }
        std::cout<<"cutPositon: "<<cutPosition<<std::endl;
        std::vector<std::vector<Triangle>> newChartsTriangles = std::vector<std::vector<Triangle>>();
        for(int k = 0; k<chartsTriangles.size();k++){
            std::cout<<k<<" ème chart étudiée dans cutMeshText"<<std::endl;
            std::vector<Triangle> chartsTriangleLeft = std::vector<Triangle>();
            std::vector<Triangle> chartsTriangleRight = std::vector<Triangle>();
            for(int i = 0; i<chartsTriangles[k].size(); i++){
                int corner0 = chartsTriangles[k][i][0];
                int corner1 = chartsTriangles[k][i][1];
                int corner2 = chartsTriangles[k][i][2];

                double pos0 = textcoords[corner0][directionOfCut];
                double pos1 = textcoords[corner1][directionOfCut];
                double pos2 = textcoords[corner2][directionOfCut];

                int numberOfNewVertices = 0;
                std::vector<int> oppositeVertices = std::vector<int>(); //stocke les sommets opposés des segments coupés
                if(pos0 < cutPosition && pos1 < cutPosition && pos2 < cutPosition){
                    chartsTriangleLeft.push_back(chartsTriangles[k][i]);
                }
                else if(pos0 > cutPosition && pos1 > cutPosition && pos2 > cutPosition){
                    chartsTriangleRight.push_back(chartsTriangles[k][i]);
                }
                else{
                    if(std::min(pos0,pos1)<cutPosition &&  std::max(pos0,pos1)>cutPosition){
                       createNewVertexTwice(wbuffer, hbuffer, corner0, corner1, directionOfCut, cutPosition);
                       numberOfNewVertices++;
                       oppositeVertices.push_back(2);
                    }

                    if(std::min(pos0,pos2)<cutPosition &&  std::max(pos0,pos2)>cutPosition){
                       createNewVertexTwice(wbuffer, hbuffer, corner0, corner2, directionOfCut, cutPosition);
                       numberOfNewVertices++;
                       oppositeVertices.push_back(1);
                    }

                    if(std::min(pos1,pos2)<cutPosition &&  std::max(pos1,pos2)>cutPosition){
                       createNewVertexTwice(wbuffer, hbuffer, corner1, corner2, directionOfCut, cutPosition);
                       numberOfNewVertices++;
                       oppositeVertices.push_back(0);
                    }

                }
                if(numberOfNewVertices == 2){
                    Triangle tri0;
                    Triangle tri1;
                    Triangle tri2;
                    if(oppositeVertices[0] == 2 && oppositeVertices[1] == 1){
                        tri0.corners[0] = corner0;
                        tri0.corners[1] = textcoords.size()-3;
                        tri0.corners[2] = textcoords.size()-1;

                        tri1.corners[0] = textcoords.size()-4;
                        tri1.corners[1] = corner1;
                        tri1.corners[2] = textcoords.size()-2;

                        tri2.corners[0] = textcoords.size()-2;
                        tri2.corners[1] = corner1;
                        tri2.corners[2] = corner2;
                        if(pos0<cutPosition){
                            chartsTriangleLeft.push_back(tri0);
                            chartsTriangleRight.push_back(tri1);
                            chartsTriangleRight.push_back(tri2);

                        }
                        else{
                            chartsTriangleRight.push_back(tri0);
                            chartsTriangleLeft.push_back(tri1);
                            chartsTriangleLeft.push_back(tri2);

                        }


                    }
                    else if(oppositeVertices[0] == 2 && oppositeVertices[1] == 0){
                        tri0.corners[0] = corner1;
                        tri0.corners[1] = textcoords.size()-1;
                        tri0.corners[2] = textcoords.size()-3;

                        tri1.corners[0] = textcoords.size()-4;
                        tri1.corners[1] = textcoords.size()-2;
                        tri1.corners[2] = corner2;

                        tri2.corners[0] = textcoords.size()-4;
                        tri2.corners[1] = corner2;
                        tri2.corners[2] = corner0;
                        if(pos1<cutPosition){
                            chartsTriangleLeft.push_back(tri0);
                            chartsTriangleRight.push_back(tri1);
                            chartsTriangleRight.push_back(tri2);

                        }
                        else{
                            chartsTriangleRight.push_back(tri0);
                            chartsTriangleLeft.push_back(tri1);
                            chartsTriangleLeft.push_back(tri2);

                        }
                    }
                    else if(oppositeVertices[0] == 1 && oppositeVertices[1] == 0){
                        tri0.corners[0] = corner2;
                        tri0.corners[1] = textcoords.size()-3;
                        tri0.corners[2] = textcoords.size()-1;

                        tri1.corners[0] = textcoords.size()-2;
                        tri1.corners[1] = textcoords.size()-4;
                        tri1.corners[2] = corner0;

                        tri2.corners[0] = textcoords.size()-2;
                        tri2.corners[1] = corner0;
                        tri2.corners[2] = corner1;
                        if(pos2<cutPosition){
                            chartsTriangleLeft.push_back(tri0);
                            chartsTriangleRight.push_back(tri1);
                            chartsTriangleRight.push_back(tri2);
                        }
                        else{
                            chartsTriangleRight.push_back(tri0);
                            chartsTriangleLeft.push_back(tri1);
                            chartsTriangleLeft.push_back(tri2);
                        }
                    }
                }
            }
            if(chartsTriangleLeft.size() ==  0 || chartsTriangleRight.size() == 0){
                std::cout<<"on remet l'ancien chart"<<std::endl;
                newChartsTriangles.push_back(chartsTriangles[k]);
            }
            else{
                std::cout<<"le chart est coupé en deux"<<std::endl;
                newChartsTriangles.push_back(chartsTriangleLeft);
                newChartsTriangles.push_back(chartsTriangleRight);
            }
        }
        std::cout<<"taille newChartsTriangles à la fin du cut: "<<newChartsTriangles.size()<<std::endl;
        chartsTriangles = newChartsTriangles;
        updateTextTriangles();
    }

    void updateChartsFromChartsTriangles(){
        charts = std::vector<std::vector<int>>();
        std::vector<int>  emptyChart = std::vector<int>();
        for(int i = 0; i<chartsTriangles.size(); i++){
            charts.push_back(emptyChart);
        }
        chartOfVertex = std::vector<int>();
        for(int k = 0; k<textcoords.size(); k++){
            chartOfVertex.push_back(-1);
        }
        for(int i = 0; i<chartsTriangles.size(); i++){
            for(int j = 0; j<chartsTriangles[i].size(); j++){
                for(int k = 0; k<3; k++){
                    chartOfVertex[chartsTriangles[i][j][k]]=i;
                }
            }
        }
        for(int k = 0; k<chartOfVertex.size();k++){
            if(chartOfVertex[k] != -1){
             charts[chartOfVertex[k]].push_back(k);
            }
        }
    }

    void createNewVertexTwice(int wbuffer, int hbuffer, int cornerA, int cornerB, int directionOfCut, double cutPosition){
        double tA = textcoords[cornerA][directionOfCut];
        double zA = textcoords[cornerA][1-directionOfCut];
        double tB = textcoords[cornerB][directionOfCut];
        double zB = textcoords[cornerB][1-directionOfCut];

        double coeff = (zB-zA)/(tB-tA);
        switch(directionOfCut){
            case 0: textcoords.push_back(uvCoord(cutPosition,coeff*(cutPosition-tA)+zA));
                    textcoords.push_back(uvCoord(cutPosition,coeff*(cutPosition-tA)+zA));
                    break;
            case 1: textcoords.push_back(uvCoord(coeff*(cutPosition-tA)+zA,cutPosition));
                    textcoords.push_back(uvCoord(coeff*(cutPosition-tA)+zA,cutPosition));
                    break;
        }

    }
    double getWidth(){
        double xmin = textcoords[0][0];
        double xmax = textcoords[0][0];
        for(int i = 1; i<textcoords.size(); i++){
            xmin = std::min(xmin, textcoords[i][0]);
            xmax = std::max(xmax, textcoords[i][0]);
        }
        return xmax-xmin;
    }
    double getHeight(){
        double ymin = textcoords[0][1];
        double ymax = textcoords[0][1];
        for(int i = 1; i<textcoords.size(); i++){
            ymin = std::min(ymin, textcoords[i][1]);
            ymax = std::max(ymax, textcoords[i][1]);
        }
        return ymax-ymin;
    }
};



#endif // PROJECTMESH_H
