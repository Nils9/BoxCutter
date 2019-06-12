#include "chart.h"

Chart::Chart()
{
    pixelsInChart = std::vector<QPoint>();
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
