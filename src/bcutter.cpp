#include "bcutter.h"

Bcutter::Bcutter()
{

}

/*void Bcutter::locateVoidBoxes(QGLFramebufferObject * _framebuffer, int wbuffer, int hbuffer, int threshold){
    //BUILD SKIPBUFFER
    QPoint skipBuffer[wbuffer][hbuffer];
    std::vector<QPoint> emptyPixels;
    std::vector<std::vector<QPoint>> emptyBoxes;
    int currentEmptySize=0;
    for(int x = 0; x<wbuffer; x++){
        for(int y = 0; y<hbuffer; y++){
            skipBuffer[x][y] = QPoint(0,0);
        }
    }
    for(int y = 0; y<hbuffer; y++){
        currentEmptySize = 0;
        for(int x = 0; x<wbuffer; x++){
            if(! hasGeometry(x,y)){
                currentEmptySize++;
                emptyPixels.push_back(QPoint(x,y));
                if(x<wbuffer-1){
                    skipBuffer[x+1][y].setX(skipBuffer[x][y].x()+1);
                }
            }
            else{
                if(currentEmptySize != 0){
                    for(int k = 1; k<currentEmptySize; k++){
                        skipBuffer[x-k-1][y].setY(k);
                    }
                    currentEmptySize = 0;
                }
            }
        }
    }

    //LOCATE LARGEST VOID BOXES
    for(int i = 0; i<emptyPixels.size(); i++){
        QPoint p = emptyPixels[i];
        int upIndex = p.y();
        int lowIndex = p.y();
        QPoint largestPossibleExtent = QPoint(skipBuffer[p.x()][p.y()].x(),skipBuffer[p.x()][p.y()].y());
        while(!hasGeometry(p.x(),upIndex) && upIndex > 0){
            upIndex--;
            largestPossibleExtent.setX(std::min(largestPossibleExtent.x(),skipBuffer[p.x()][upIndex].x()));
            largestPossibleExtent.setY(std::min(largestPossibleExtent.y(),skipBuffer[p.x()][upIndex].y()));
        }
        while(!hasGeometry(p.x(),lowIndex) && lowIndex < hbuffer-1){
            lowIndex++;
            largestPossibleExtent.setX(std::min(largestPossibleExtent.x(),skipBuffer[p.x()][lowIndex].x()));
            largestPossibleExtent.setY(std::min(largestPossibleExtent.y(),skipBuffer[p.x()][lowIndex].y()));
        }
        if((largestPossibleExtent.y()+largestPossibleExtent.x()+1)*((lowIndex-1)-(upIndex+1))>threshold){
           std::vector<QPoint> emptyBox;
           emptyBox.push_back(QPoint(p.x()-largestPossibleExtent.x(),upIndex+1));
           emptyBox.push_back(QPoint(p.x()+largestPossibleExtent.y(),lowIndex-1));
           emptyBoxes.push_back(emptyBox);
        }
    }

std::cout<<emptyBoxes.size()<<std::endl;

QImage image = _framebuffer->toImage();
for(int i = 0; i<emptyPixels.size();i++){
    image.setPixelColor(emptyPixels[i],QColor(255,0,0));
}
for(int i = 0; i<emptyBoxes.size();i++){
    QPoint upLeft = emptyBoxes[i][0];
    QPoint lowRight = emptyBoxes[i][1];
    //std::cout<<"x: "<<upLeft.x()<<" y:"<<upLeft.y()<<" X:"<<lowRight.x()<<" Y:"<<lowRight.y()<<std::endl;
    if(lowRight.y()>hbuffer){
        std::cout<<"lowRight.y(): "<<lowRight.y()<<std::endl;
    }
    for(int x = upLeft.x(); x<=lowRight.x(); x++){
        for(int y = upLeft.y(); y<=lowRight.y(); y++){
            image.setPixelColor(x,y,QColor(0,0,0));
        }
    }
    //image.setPixelColor(upLeft, QColor(0,255,0));
    //image.setPixelColor(lowRight,QColor(255,0,0));
}
image.save("test.jpg");
}

bool Bcutter::hasGeometry(int x, int y){
    GLubyte data[4];
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
    int index = ((data[0] << 16) + (data[1] << 8) + data[2]);
    return !(index == 0xFFFFFF);
}*/
