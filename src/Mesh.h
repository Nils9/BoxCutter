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
    std::vector<std::vector<Triangle>> chartsTriangles3D;
    std::vector<std::vector<double>> chartsBoundingBoxes;
    std::vector<int> chartOfVertex;
    double meshArea;

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
        triangles_text = newTrianglesText;
    }

    void updateTriangles(){
        std::vector<Triangle> newTriangles = std::vector<Triangle>();
        for(int k = 0; k<chartsTriangles3D.size(); k++){
            for(int i = 0; i<chartsTriangles3D[k].size(); i++){
                newTriangles.push_back(chartsTriangles3D[k][i]);
            }
        }
        triangles = newTriangles;
    }

    void invertXY(int chartIndex){
        for(int i = 0; i<charts[chartIndex].size(); i++){
            textcoords[charts[chartIndex][i]] = uvCoord(textcoords[charts[chartIndex][i]][1],textcoords[charts[chartIndex][i]][0]);
        }
    }
    void moveVertices(int chartIndex, double deltaX, double deltaY){
        for(int i = 0; i<charts[chartIndex].size(); i++){
            textcoords[charts[chartIndex][i]] = uvCoord(textcoords[charts[chartIndex][i]][0]+deltaX,textcoords[charts[chartIndex][i]][1]+deltaY);
        }
    }
    void updateBoundingBoxes(){
        std::vector<std::vector<double>> boundingBoxes = std::vector<std::vector<double>>();
        for(int i = 0; i<chartsTriangles.size(); i++){
            if(charts[i].size() != 0){
                std::vector<double> bbox = std::vector<double>();
                bbox.push_back(textcoords[charts[i][0]][0]);
                bbox.push_back(textcoords[charts[i][0]][1]);
                bbox.push_back(textcoords[charts[i][0]][0]);
                bbox.push_back(textcoords[charts[i][0]][1]);
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
        chartsBoundingBoxes =  boundingBoxes;
    }

    double cutMeshText(int wbuffer, int hbuffer, int directionOfCut, int cutPositionPixel){
        double cutPosition;
        switch(directionOfCut){
            case 0: cutPosition = getXMin()+getWidth()*((double) cutPositionPixel/(double)wbuffer);
                    break;
            case 1: cutPosition = getYMin()+getHeight()*((double)cutPositionPixel/(double)hbuffer);
                    break;
        }
        std::vector<std::vector<Triangle>> newChartsTriangles = std::vector<std::vector<Triangle>>();
        std::vector<std::vector<Triangle>> newChartsTriangles3D = std::vector<std::vector<Triangle>>();
        double cutLength = 0;
        for(int k = 0; k<chartsTriangles.size();k++){
            std::vector<Triangle> chartsTriangleLeft = std::vector<Triangle>();
            std::vector<Triangle> chartsTriangleRight = std::vector<Triangle>();
            std::vector<Triangle> chartsTriangleLeft3D = std::vector<Triangle>();
            std::vector<Triangle> chartsTriangleRight3D = std::vector<Triangle>();
            for(int i = 0; i<chartsTriangles[k].size(); i++){
                int corner0 = chartsTriangles[k][i][0];
                int corner1 = chartsTriangles[k][i][1];
                int corner2 = chartsTriangles[k][i][2];

                double pos0 = textcoords[corner0][directionOfCut];
                double pos1 = textcoords[corner1][directionOfCut];
                double pos2 = textcoords[corner2][directionOfCut];

                int corner3D0 = chartsTriangles3D[k][i][0];
                int corner3D1 = chartsTriangles3D[k][i][1];
                int corner3D2 = chartsTriangles3D[k][i][2];


                int numberOfNewVertices = 0;
                std::vector<int> oppositeVertices = std::vector<int>(); //stocke les sommets opposés des segments coupés
                if(pos0 < cutPosition && pos1 < cutPosition && pos2 < cutPosition){
                    chartsTriangleLeft.push_back(chartsTriangles[k][i]);
                    chartsTriangleLeft3D.push_back(chartsTriangles3D[k][i]);
                }
                else if(pos0 > cutPosition && pos1 > cutPosition && pos2 > cutPosition){
                    chartsTriangleRight.push_back(chartsTriangles[k][i]);
                    chartsTriangleRight3D.push_back(chartsTriangles3D[k][i]);
                }
                else{
                    if(std::min(pos0,pos1)<cutPosition &&  std::max(pos0,pos1)>cutPosition){
                       double distance = createNewVertexTwice(wbuffer, hbuffer, corner0, corner1, directionOfCut, cutPosition);
                       createNewVertex3D(wbuffer, hbuffer, corner3D0, corner3D1, distance);
                       numberOfNewVertices++;
                       oppositeVertices.push_back(2);
                    }

                    if(std::min(pos0,pos2)<cutPosition &&  std::max(pos0,pos2)>cutPosition){
                       double distance = createNewVertexTwice(wbuffer, hbuffer, corner0, corner2, directionOfCut, cutPosition);
                       createNewVertex3D(wbuffer, hbuffer, corner3D0, corner3D2, distance);
                       numberOfNewVertices++;
                       oppositeVertices.push_back(1);
                    }

                    if(std::min(pos1,pos2)<cutPosition &&  std::max(pos1,pos2)>cutPosition){
                       double distance = createNewVertexTwice(wbuffer, hbuffer, corner1, corner2, directionOfCut, cutPosition);
                       createNewVertex3D(wbuffer, hbuffer, corner3D1, corner3D2, distance);
                       numberOfNewVertices++;
                       oppositeVertices.push_back(0);
                    }

                }
                if(numberOfNewVertices == 2){
                    Triangle tri0;
                    Triangle tri1;
                    Triangle tri2;

                    Triangle tri3D0;
                    Triangle tri3D1;
                    Triangle tri3D2;
                    if(oppositeVertices[0] == 2 && oppositeVertices[1] == 1){
                        tri0.corners[0] = corner0;
                        tri0.corners[1] = textcoords.size()-3;
                        tri0.corners[2] = textcoords.size()-1;

                        tri3D0.corners[0] = corner3D0;
                        tri3D0.corners[1] = vertices.size()-2;
                        tri3D0.corners[2] = vertices.size()-1;

                        tri1.corners[0] = textcoords.size()-4;
                        tri1.corners[1] = corner1;
                        tri1.corners[2] = textcoords.size()-2;

                        tri3D1.corners[0] = vertices.size()-2;
                        tri3D1.corners[1] = corner3D1;
                        tri3D1.corners[2] = vertices.size()-1;

                        tri2.corners[0] = textcoords.size()-2;
                        tri2.corners[1] = corner1;
                        tri2.corners[2] = corner2;

                        tri3D2.corners[0] = vertices.size()-1;
                        tri3D2.corners[1] = corner3D1;
                        tri3D2.corners[2] = corner3D2;

                        if(pos0<cutPosition){
                            chartsTriangleLeft.push_back(tri0);
                            chartsTriangleRight.push_back(tri1);
                            chartsTriangleRight.push_back(tri2);

                            chartsTriangleLeft3D.push_back(tri3D0);
                            chartsTriangleRight3D.push_back(tri3D1);
                            chartsTriangleRight3D.push_back(tri3D2);

                        }
                        else{
                            chartsTriangleRight.push_back(tri0);
                            chartsTriangleLeft.push_back(tri1);
                            chartsTriangleLeft.push_back(tri2);

                            chartsTriangleRight3D.push_back(tri3D0);
                            chartsTriangleLeft3D.push_back(tri3D1);
                            chartsTriangleLeft3D.push_back(tri3D2);

                        }


                    }
                    else if(oppositeVertices[0] == 2 && oppositeVertices[1] == 0){
                        tri0.corners[0] = corner1;
                        tri0.corners[1] = textcoords.size()-1;
                        tri0.corners[2] = textcoords.size()-3;

                        tri3D0.corners[0] = corner3D1;
                        tri3D0.corners[1] = vertices.size()-1;
                        tri3D0.corners[2] = vertices.size()-2;

                        tri1.corners[0] = textcoords.size()-4;
                        tri1.corners[1] = textcoords.size()-2;
                        tri1.corners[2] = corner2;

                        tri3D1.corners[0] = vertices.size()-2;
                        tri3D1.corners[1] = vertices.size()-1;
                        tri3D1.corners[2] = corner3D2;

                        tri2.corners[0] = textcoords.size()-4;
                        tri2.corners[1] = corner2;
                        tri2.corners[2] = corner0;

                        tri3D2.corners[0] = vertices.size()-2;
                        tri3D2.corners[1] = corner3D2;
                        tri3D2.corners[2] = corner3D0;

                        if(pos1<cutPosition){
                            chartsTriangleLeft.push_back(tri0);
                            chartsTriangleRight.push_back(tri1);
                            chartsTriangleRight.push_back(tri2);

                            chartsTriangleLeft3D.push_back(tri3D0);
                            chartsTriangleRight3D.push_back(tri3D1);
                            chartsTriangleRight3D.push_back(tri3D2);

                        }
                        else{
                            chartsTriangleRight.push_back(tri0);
                            chartsTriangleLeft.push_back(tri1);
                            chartsTriangleLeft.push_back(tri2);

                            chartsTriangleRight3D.push_back(tri3D0);
                            chartsTriangleLeft3D.push_back(tri3D1);
                            chartsTriangleLeft3D.push_back(tri3D2);

                        }
                    }
                    else if(oppositeVertices[0] == 1 && oppositeVertices[1] == 0){
                        tri0.corners[0] = corner2;
                        tri0.corners[1] = textcoords.size()-3;
                        tri0.corners[2] = textcoords.size()-1;

                        tri3D0.corners[0] = corner3D2;
                        tri3D0.corners[1] = vertices.size()-2;
                        tri3D0.corners[2] = vertices.size()-1;

                        tri1.corners[0] = textcoords.size()-2;
                        tri1.corners[1] = textcoords.size()-4;
                        tri1.corners[2] = corner0;

                        tri3D1.corners[0] = vertices.size()-1;
                        tri3D1.corners[1] = vertices.size()-2;
                        tri3D1.corners[2] = corner3D0;

                        tri2.corners[0] = textcoords.size()-2;
                        tri2.corners[1] = corner0;
                        tri2.corners[2] = corner1;

                        tri3D2.corners[0] = vertices.size()-1;
                        tri3D2.corners[1] = corner3D0;
                        tri3D2.corners[2] = corner3D1;

                        if(pos2<cutPosition){
                            chartsTriangleLeft.push_back(tri0);
                            chartsTriangleRight.push_back(tri1);
                            chartsTriangleRight.push_back(tri2);

                            chartsTriangleLeft3D.push_back(tri3D0);
                            chartsTriangleRight3D.push_back(tri3D1);
                            chartsTriangleRight3D.push_back(tri3D2);
                        }
                        else{
                            chartsTriangleRight.push_back(tri0);
                            chartsTriangleLeft.push_back(tri1);
                            chartsTriangleLeft.push_back(tri2);

                            chartsTriangleRight3D.push_back(tri3D0);
                            chartsTriangleLeft3D.push_back(tri3D1);
                            chartsTriangleLeft3D.push_back(tri3D2);
                        }
                    }
                    cutLength += fabs(textcoords[textcoords.size()-1][1-directionOfCut] - textcoords[textcoords.size()-3][1-directionOfCut]);
                }
            }
            if(chartsTriangleLeft.size() ==  0 || chartsTriangleRight.size() == 0){
                newChartsTriangles.push_back(chartsTriangles[k]);
                newChartsTriangles3D.push_back(chartsTriangles3D[k]);
            }
            else{
                newChartsTriangles.push_back(chartsTriangleLeft);
                newChartsTriangles.push_back(chartsTriangleRight);
                newChartsTriangles3D.push_back(chartsTriangleLeft3D);
                newChartsTriangles3D.push_back(chartsTriangleRight3D);
            }
        }
        chartsTriangles = newChartsTriangles;
        chartsTriangles3D = newChartsTriangles3D;
        updateTextTriangles();
        updateTriangles();
        return cutLength;
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

    double createNewVertexTwice(int wbuffer, int hbuffer, int cornerA, int cornerB, int directionOfCut, double cutPosition){
        double tA = textcoords[cornerA][directionOfCut];
        double zA = textcoords[cornerA][1-directionOfCut];
        double tB = textcoords[cornerB][directionOfCut];
        double zB = textcoords[cornerB][1-directionOfCut];

        double coeff = (zB-zA)/(tB-tA);
        uvCoord newVertex = uvCoord(0.,0.);
        switch(directionOfCut){
            case 0: newVertex = uvCoord(cutPosition,coeff*(cutPosition-tA)+zA);
                    break;

            case 1: newVertex = uvCoord(coeff*(cutPosition-tA)+zA,cutPosition);
                    break;
        }
        textcoords.push_back(newVertex);
        textcoords.push_back(newVertex);
        double distanceNew = (newVertex[0]-textcoords[cornerA][0])*(newVertex[0]-textcoords[cornerA][0])+(newVertex[1]-textcoords[cornerA][1])*(newVertex[1]-textcoords[cornerA][1]);
        double distanceAB = (textcoords[cornerB][0]-textcoords[cornerA][0])*(textcoords[cornerB][0]-textcoords[cornerA][0])+(textcoords[cornerB][1]-textcoords[cornerA][1])*(textcoords[cornerB][1]-textcoords[cornerA][1]);
        return(distanceNew/distanceAB);

    }

    void createNewVertex3D(int hbuffer, int wbuffer, int corner3DA, int corner3DB, double distance){
        double xA = vertices[corner3DA][0];
        double yA = vertices[corner3DA][1];
        double zA = vertices[corner3DA][2];

        double xB = vertices[corner3DB][0];
        double yB = vertices[corner3DB][1];
        double zB = vertices[corner3DB][2];

        vertices.push_back(Vertex(xA+distance*(xB-xA),yA+distance*(yB-yA),zA+distance*(zB-zA)));
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

    double getXMin(){
        double xmin = textcoords[0][0];
        for(int i = 1; i<textcoords.size(); i++){
            xmin = std::min(xmin, textcoords[i][0]);
        }
        return xmin;
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

    double getYMin(){
        double ymin = textcoords[0][1];
        for(int i = 1; i<textcoords.size(); i++){
            ymin = std::min(ymin, textcoords[i][1]);
        }
        return ymin;
    }

    double getMeshArea(){
        double area = 0;
        for(int i = 0; i < triangles_text.size(); i++){
            int p0 = triangles_text[i][0];
            int p1 = triangles_text[i][1];
            int p2 = triangles_text[i][2];
            float x0 = textcoords[p0][0];
            float y0 = textcoords[p0][1];
            float x1 = textcoords[p1][0];
            float y1 = textcoords[p1][1];
            float x2 = textcoords[p2][0];
            float y2 = textcoords[p2][1];
            area += fabs(0.5*(x0*(y1-y2)+x1*(y2-y0)+x2*(y0-y1)));
        }
        return area;
    }
};



#endif // PROJECTMESH_H
