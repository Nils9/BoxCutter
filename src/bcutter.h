#ifndef BCUTTER_H
#define BCUTTER_H
#include <iostream>
#include <vector>
#include <QImage>
#include <QGLFramebufferObject>


class Bcutter
{
private:
    std::vector<std::vector<int>> largestVoidBoxes;

public:
    Bcutter();
    void locateVoidBoxes(QGLFramebufferObject * _framebuffer, int wbuffer, int hbuffer, int threshold);
    void hasGeometry(int x, int y);

};

#endif // BCUTTER_H
