#include "chart.h"

/*Chart::Chart()
{
    vertices = std::vector<int>();
    triangles = std::vector<Triangle>();
    boundingBox = std::vector<double>();
}

std::vector<double> Chart::getBoundingBox(){
    if(boundingBox.size()==0 & pixelsInChart.size()!=0){
        int minX = pixelsInChart[0].x();
        int minY = pixelsInChart[0].y();
        int maxX = pixelsInChart[0].x();
        int maxY = pixelsInChart[0].y();
        for(int i = 1; i<pixelsInChart.size(); i++){
            minX = std::min(minX,pixelsInChart[i].x());
            minY = std::min(minY, pixelsInChart[i].y());
            maxX = std::max(maxX, pixelsInChart[i].x());
            maxY = std::max(maxY, pixelsInChart[i].y());
        }
        boundingBox.push_back(QPoint(minX,minY));
        boundingBox.push_back(QPoint(maxX,maxY));
    }
    std::vector<QPoint> copy = std::vector<QPoint>();
    for(int i = 0; i<boundingBox.size(); i++){
        copy.push_back(boundingBox[i]);
    }
    return copy;
}*/




Chart::Chart()
{
    pixelsInChart = std::vector<QPoint>();
    boundingBox = std::vector<QPoint>();
}

void Chart::addPixelInChart(QPoint p){
    pixelsInChart.push_back(p);
}

std::vector<QPoint> Chart::getPixelsInChart(){
    std::vector<QPoint> copy = std::vector<QPoint>();
    for(int i = 0; i<pixelsInChart.size(); i++){
        copy.push_back(pixelsInChart[i]);
    }
    return copy;
}

std::vector<QPoint> Chart::getBoundingBox(){
    if(boundingBox.size()==0 & pixelsInChart.size()!=0){
        int minX = pixelsInChart[0].x();
        int minY = pixelsInChart[0].y();
        int maxX = pixelsInChart[0].x();
        int maxY = pixelsInChart[0].y();
        for(int i = 1; i<pixelsInChart.size(); i++){
            minX = std::min(minX,pixelsInChart[i].x());
            minY = std::min(minY, pixelsInChart[i].y());
            maxX = std::max(maxX, pixelsInChart[i].x());
            maxY = std::max(maxY, pixelsInChart[i].y());
        }
        boundingBox.push_back(QPoint(minX,minY));
        boundingBox.push_back(QPoint(maxX,maxY));
    }
    std::vector<QPoint> copy = std::vector<QPoint>();
    for(int i = 0; i<boundingBox.size(); i++){
        copy.push_back(boundingBox[i]);
    }
    return copy;
}

std::vector<int> Chart::getMeshTextChartIndices(){
    std::vector<int> copy = std::vector<int>();
    for(int i = 0; i<meshTextChartIndices.size(); i++){
        copy.push_back(meshTextChartIndices[i]);
    }
    return copy;
}

std::vector<Chart*> Chart::splitChart(int a, int b, int t){
    std::vector<QPoint> bB = this->getBoundingBox();
    std::vector<QPoint> pixelsInChart = this->getPixelsInChart();

    Chart * chart1 = new Chart();
    Chart * chart2 = new Chart();
    Chart * chart3 = new Chart();
    std::vector<Chart*> sons;

    if(t == 0){ //vertical
        for(int i = 0; i < pixelsInChart.size(); i++){
            QPoint p = pixelsInChart[i];
            if(p.x() < a){
                chart1->addPixelInChart(p);
            }
            else if(a <= p.x() && p.x() <= b){
                chart2->addPixelInChart(p);
            }
            else if(b < p.x()){
                chart3->addPixelInChart(p);
            }
        }

    }

    if(t == 1){ //horizontal
        for(int i = 0; i < pixelsInChart.size(); i++){
            QPoint p = pixelsInChart[i];
            if(p.y() < a){
                chart1->addPixelInChart(p);
            }
            else if(a <= p.y() && p.y() <= b){
                chart2->addPixelInChart(p);
            }
            else if(b < p.y()){
                chart3->addPixelInChart(p);
            }
        }
    }

    std::vector<QPoint> pixels1 = chart1->getPixelsInChart();
    if(pixels1.size() > 0){
        sons.push_back(chart1);
    }
    std::vector<QPoint> pixels2 = chart2->getPixelsInChart();
    if(pixels2.size() > 0){
        sons.push_back(chart2);
    }
    std::vector<QPoint> pixels3 = chart3->getPixelsInChart();
    if(pixels3.size() > 0){
        sons.push_back(chart3);
    }
    return sons;
}
