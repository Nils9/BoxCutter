#ifndef CHART_H
#define CHART_H

#include <vector>
#include <iostream>
#include <QPoint>


class Chart
{
private:
    std::vector<QPoint> pixelsInChart;
public:
    Chart();
    void addPixelInChart(QPoint p);
    std::vector<QPoint> getPixelsInChart();
};

#endif // CHART_H
