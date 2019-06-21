#ifndef CHART_H
#define CHART_H

#include <vector>
#include <iostream>
#include <QPoint>
#include "Mesh.h";


class Chart
{
private:
    /*Mesh mesh;
    std::vector<int> vertices;
    std::vector<Triangle> triangles;
    std::vector<double> boundingBox;*/

    std::vector<QPoint> pixelsInChart;
    std::vector<QPoint> boundingBox;
    //int meshTextChartIndex;
    std::vector<int> meshTextChartIndices;

public:
    Chart();
    //std::vector<double> getBoundingBox();


    void addPixelInChart(QPoint p);
    std::vector<QPoint> getPixelsInChart();
    std::vector<QPoint> getBoundingBox();
    //int getMeshTextChartIndex(){return meshTextChartIndex;}
    //void setMeshTextChartIndex(int index){meshTextChartIndex = index;}
    std::vector<int> getMeshTextChartIndices();
    void addMeshTextChartIndices(int i){meshTextChartIndices.push_back(i);}
    std::vector<Chart*> splitChart(int a, int b, int typeCut);
};

#endif // CHART_H
