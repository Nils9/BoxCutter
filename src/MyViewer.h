#ifndef MYVIEWER_H
#define MYVIEWER_H

// Mesh stuff:
#include "Mesh.h"

// Parsing:
#include "BasicIO.h"

// opengl and basic gl utilities:
#define GL_GLEXT_PROTOTYPES
#include <gl/openglincludeQtComp.h>
#include <GL/glext.h>
#include <QOpenGLFunctions_3_0>
#include <QOpenGLFunctions>
#include <QGLViewer/qglviewer.h>
#include <QGLFramebufferObject>

#include <gl/GLUtilityMethods.h>

// Qt stuff:
#include <QFormLayout>
#include <QToolBar>
#include <QColorDialog>
#include <QFileDialog>
#include <QKeyEvent>
#include <QInputDialog>
#include <QLineEdit>


#include "qt/QSmartAction.h"

#include "packingtree.h"
#include "chart.h"

class MyViewer : public QGLViewer , public QOpenGLFunctions_3_0
{
    Q_OBJECT

    Mesh mesh;

    QWidget * controls;

    QImage atlas;

    QImage voidBoxesImage;

    std::vector<Chart *> charts = std::vector<Chart *>();

    std::vector<std::vector<bool>> examinedPixels = std::vector<std::vector<bool>>();

    std::vector<std::vector<int>> chartOfPixels = std::vector<std::vector<int>>();

public :

    void renderGeometry(int wbuffer, int hbuffer, int threshold = 10)
        {
            QGLFramebufferObject * _framebuffer = new QGLFramebufferObject(QSize(wbuffer , hbuffer), QGLFramebufferObject::Depth);

            _framebuffer->bind();
            glViewport(0, 0, _framebuffer->width(), _framebuffer->height());

            glPushAttrib( GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT );
            glPushAttrib(GL_COLOR_BUFFER_BIT | GL_VIEWPORT_BIT | GL_ENABLE_BIT);
            glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

            glDisable(GL_BLEND);
            glDisable(GL_LIGHTING);
            glShadeModel(GL_FLAT);
            glEnable(GL_DEPTH);

            float back[4];
            glGetFloatv( GL_COLOR_CLEAR_VALUE , back );

            glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // SETUP THE CAMERA TO DRAW "IN 2D", AND DRAW VALUES:
            {
                glPolygonMode( GL_FRONT_AND_BACK , GL_FILL );

                glMatrixMode( GL_PROJECTION );
                glPushMatrix();
                glLoadIdentity();
                glOrtho( 0.f , 1.f , 0.f , 1.f , -1.f , 1.f );
                glMatrixMode( GL_MODELVIEW );
                glPushMatrix();
                glLoadIdentity();

                // DRAW UV TRIANGLES:
                draw();

                glPopMatrix();
                glMatrixMode( GL_PROJECTION );
                glPopMatrix();
                glMatrixMode( GL_MODELVIEW );
            }

            // TO READ THE VALUES :
            atlas = _framebuffer->toImage();

            glClearColor(back[0], back[1], back[2], back[3]);
            glPopAttrib();
            _framebuffer->release();
            delete _framebuffer;
        }

    bool hasGeometry(int x, int y){
        GLubyte data[4];
        glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
        int index = ((data[0] << 16) + (data[1] << 8) + data[2]);
        return !(index == 0xFFFFFF);
    }

    std::vector<std::vector<QPoint>> locateVoidBoxes(int wbuffer, int hbuffer, int threshold){
        QPoint skipBuffer[wbuffer][hbuffer];
        std::vector<QPoint> emptyPixels;
        std::vector<QPoint> fullPixels;
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
                if(atlas.pixelColor(x,y).red() == 255 && atlas.pixelColor(x,y).green() == 255 && atlas.pixelColor(x,y).blue() == 255){
                    currentEmptySize++;
                    //emptyPixels.push_back(QPoint(x,hbuffer-1-y));
                    emptyPixels.push_back(QPoint(x,y));
                    if(x<wbuffer-1){
                        skipBuffer[x+1][y].setX(skipBuffer[x][y].x()+1);
                    }
                }
                else{
                    fullPixels.push_back(QPoint(x,y));
                    //fullPixels.push_back(QPoint(x,hbuffer-1-y));
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
        std::cout<<"emptyPixels.size(): "<<emptyPixels.size()<<std::endl;
        for(int i = 0; i<emptyPixels.size(); i++){
            QPoint p = emptyPixels[i];
            int upIndex = p.y();
            int lowIndex = p.y();
            QPoint largestPossibleExtent = QPoint(skipBuffer[p.x()][p.y()].x(),skipBuffer[p.x()][p.y()].y());
            while(atlas.pixelColor(p.x(),upIndex).red() == 255 && atlas.pixelColor(p.x(),upIndex).green() == 255 && atlas.pixelColor(p.x(),upIndex).blue() == 255 && upIndex > 0){
                upIndex--;
                largestPossibleExtent.setX(std::min(largestPossibleExtent.x(),skipBuffer[p.x()][upIndex].x()));
                largestPossibleExtent.setY(std::min(largestPossibleExtent.y(),skipBuffer[p.x()][upIndex].y()));
            }
            while(atlas.pixelColor(p.x(),lowIndex).red() == 255 && atlas.pixelColor(p.x(),lowIndex).green() == 255 && atlas.pixelColor(p.x(),lowIndex).blue() == 255 && lowIndex < hbuffer-1){
                lowIndex++;
                largestPossibleExtent.setX(std::min(largestPossibleExtent.x(),skipBuffer[p.x()][lowIndex].x()));
                largestPossibleExtent.setY(std::min(largestPossibleExtent.y(),skipBuffer[p.x()][lowIndex].y()));
            }
            if((largestPossibleExtent.y()+largestPossibleExtent.x()+1)*((lowIndex-1)-(upIndex+1))>threshold){
               std::vector<QPoint> emptyBox;
               emptyBox.push_back(QPoint(p.x()-largestPossibleExtent.x(),upIndex));
               emptyBox.push_back(QPoint(p.x()+largestPossibleExtent.y(),lowIndex));
               //emptyBox.push_back(QPoint(p.x()-largestPossibleExtent.x(),hbuffer-1-lowIndex));
               //emptyBox.push_back(QPoint(p.x()+largestPossibleExtent.y(),hbuffer-1-upIndex));
               emptyBoxes.push_back(emptyBox);
            }
        }
        struct {
                bool operator()(std::vector<QPoint> emptyBox, std::vector<QPoint> emptyBoxPrim) const  //opérateur qui compare les tailles de deux emptyBox pour trier notre liste
                {
                    int x = emptyBox[0].x();
                    int y = emptyBox[0].y();
                    int X = emptyBox[1].x();
                    int Y = emptyBox[1].y();

                    int xPrim = emptyBoxPrim[0].x();
                    int yPrim = emptyBoxPrim[0].y();
                    int XPrim = emptyBoxPrim[1].x();
                    int YPrim = emptyBoxPrim[1].y();

                    return (X-x)*(Y-y)>(XPrim-xPrim)*(YPrim-yPrim);
                }
            } sizeIsBigger;
        std::sort(emptyBoxes.begin(),emptyBoxes.end(),sizeIsBigger);
        std::cout<<"emptyBoxes.size() before filter overlap: "<<emptyBoxes.size()<<std::endl;
        //FILTER OVERLAPS
        int i = 0;
        while(i<emptyBoxes.size()-1){
            int j = i+1;
            while(j<emptyBoxes.size()){
                float overlap = overlapAmount(emptyBoxes[i],emptyBoxes[j]);
                if(overlap>0.1){
                    emptyBoxes.erase(emptyBoxes.begin()+j);
                }
                else{
                    j++;
                }
            }
            i++;
        }
        std::cout<<"emptyBoxes.size() after filter overlap: "<<emptyBoxes.size()<<std::endl;
        //CREATE IMAGE
        voidBoxesImage = atlas.copy();
        for(int i = 0; i<emptyBoxes.size();i++){
            QPoint upLeft = emptyBoxes[i][0];
            QPoint lowRight = emptyBoxes[i][1];
            std::cout<<"x: "<<upLeft.x()<<" y:"<<upLeft.y()<<" X:"<<lowRight.x()<<" Y:"<<lowRight.y()<<std::endl;
            for(int x = upLeft.x(); x<=lowRight.x(); x++){
                for(int y = upLeft.y(); y<=lowRight.y(); y++){
                    voidBoxesImage.setPixelColor(x,y,QColor(0,0,0));
                }
            }
        }
        return emptyBoxes;
    }


    /*std::vector<std::vector<QPoint>> locateVoidBoxes(QGLFramebufferObject * _framebuffer, int wbuffer, int hbuffer, int threshold){
        //BUILD SKIPBUFFER
        QPoint skipBuffer[wbuffer][hbuffer];
        std::vector<QPoint> emptyPixels;
        std::vector<QPoint> fullPixels;
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
                    //emptyPixels.push_back(QPoint(x,y));
                    emptyPixels.push_back(QPoint(x,hbuffer-1-y));
                    if(x<wbuffer-1){
                        skipBuffer[x+1][y].setX(skipBuffer[x][y].x()+1);
                    }
                }
                else{
                    //fullPixels.push_back(QPoint(x,y));
                    fullPixels.push_back(QPoint(x,hbuffer-1-y));
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
               //emptyBox.push_back(QPoint(p.x()-largestPossibleExtent.x(),upIndex));
               //emptyBox.push_back(QPoint(p.x()+largestPossibleExtent.y(),lowIndex));
               emptyBox.push_back(QPoint(p.x()-largestPossibleExtent.x(),hbuffer-1-lowIndex));
               emptyBox.push_back(QPoint(p.x()+largestPossibleExtent.y(),hbuffer-1-upIndex));
               emptyBoxes.push_back(emptyBox);
            }
        }
        struct {
                bool operator()(std::vector<QPoint> emptyBox, std::vector<QPoint> emptyBoxPrim) const  //opérateur qui compare les tailles de deux emptyBox pour trier notre liste
                {
                    int x = emptyBox[0].x();
                    int y = emptyBox[0].y();
                    int X = emptyBox[1].x();
                    int Y = emptyBox[1].y();

                    int xPrim = emptyBoxPrim[0].x();
                    int yPrim = emptyBoxPrim[0].y();
                    int XPrim = emptyBoxPrim[1].x();
                    int YPrim = emptyBoxPrim[1].y();

                    return (X-x)*(Y-y)>(XPrim-xPrim)*(YPrim-yPrim);
                }
            } sizeIsBigger;
    std::sort(emptyBoxes.begin(),emptyBoxes.end(),sizeIsBigger);
    //FILTER OVERLAPS
    int i = 0;
    while(i<emptyBoxes.size()-1){
        int j = i+1;
        while(j<emptyBoxes.size()){
            float overlap = overlapAmount(emptyBoxes[i],emptyBoxes[j]);
            if(overlap>0.1){
                emptyBoxes.erase(emptyBoxes.begin()+j);
            }
            else{
                j++;
            }
        }
        i++;
    }
    QImage image = _framebuffer->toImage();
    atlas = QImage(wbuffer,hbuffer, QImage::Format_RGB32);
    atlas.fill(QColor(0,0,0));
    for(int i = 0; i<emptyPixels.size();i++){
        image.setPixelColor(emptyPixels[i],QColor(255,0,0));
    }
    for(int i = 0; i<fullPixels.size(); i++){
        atlas.setPixelColor(fullPixels[i], QColor(255,255,255));
    }
    atlas.save("atlas.jpg");
    //emptyBoxes.erase(emptyBoxes.begin());
    for(int i = 0; i<emptyBoxes.size();i++){
        QPoint upLeft = emptyBoxes[i][0];
        QPoint lowRight = emptyBoxes[i][1];
        std::cout<<"x: "<<upLeft.x()<<" y:"<<upLeft.y()<<" X:"<<lowRight.x()<<" Y:"<<lowRight.y()<<std::endl;
        for(int x = upLeft.x(); x<=lowRight.x(); x++){
            for(int y = upLeft.y(); y<=lowRight.y(); y++){
                image.setPixelColor(x,y,QColor(0,0,0));
            }
        }
    }

    image.save("voidBoxes.jpg");

    return emptyBoxes;
    }*/

    int emptyBoxSize(std::vector<QPoint> emptyBox){
        int x = emptyBox[0].x();
        int y = emptyBox[0].y();
        int X = emptyBox[1].x();
        int Y = emptyBox[1].y();
        return (X-x)*(Y-y);
    }

    //calcule le pourcentage d'overlap entre deux empty boxes
    float overlapAmount(std::vector<QPoint> emptyBox, std::vector<QPoint> emptyBoxPrim){
        int x = emptyBox[0].x();
        int y = emptyBox[0].y();
        int X = emptyBox[1].x();
        int Y = emptyBox[1].y();

        int xPrim = emptyBoxPrim[0].x();
        int yPrim = emptyBoxPrim[0].y();
        int XPrim = emptyBoxPrim[1].x();
        int YPrim = emptyBoxPrim[1].y();

        if(((X>=xPrim && X<=XPrim)||(XPrim>=x && XPrim<=X))&&((Y>=yPrim && Y<=YPrim)||(YPrim>=y && YPrim<=Y))){
            int overlapArea = (std::min(X,XPrim)-std::max(x,xPrim)+1)*(std::min(Y,YPrim)-std::max(y,yPrim)+1);
            int minArea = std::min((X-x+1)*(Y-y+1),(XPrim-xPrim+1)*(YPrim-yPrim+1));
            return (float) overlapArea/ (float) minArea;

        }
        else{
            return 0;
        }
    }



    //TESTEE : Calcule la longueur d'une coupe verticale (o=0) ou horizontale (o=1)
        int computeLength(int hbuffer, int wbuffer, int x, int o){
           int count = 0;
           switch(o){
            case 0:
                for(int i = 0; i < wbuffer; i++){
                    if(hasGeometry(x, i)){
                        count += 1;
                    }
                }
            case 1:
                for(int i = 0; i < hbuffer; i++){
                    if(hasGeometry(i, x)){
                        count += 1;
                    }
                }
           }
        return count;
        }

        //Pour une coupe donnée, renvoie la position optimale de coupe à +/- 5% de la coupe de départ
        std::vector<int> optimizeCut(int hbuffer, int wbuffer, std::vector<QPoint> vbox){
            int x0 = vbox[0].x();
            int y0 = vbox[0].y();
            int x1 = vbox[1].x();
            int y1 = vbox[1].y();
            int width = x1 - x0;
            int height = y1 - y0;
            float w0 = int(0.05 * width);
            float h0 = int(0.05 * height);

            //Trouver les coupes horizontales optimales
            int bestCut0 = NULL;
            int bestLength = hbuffer+1;
            for(int i = std::max(int(y0 - h0), 0); i < y0 + h0 + 1; i++){
                int currentLength = computeLength(hbuffer, wbuffer, i, 1);
                if((currentLength <= bestLength) && currentLength >= 0){
                    bestLength = currentLength;
                    bestCut0 = i;
                    std::cout<<"bestCut0 devient "<<i<<std::endl;
                }
            }

            int bestCut1 = NULL;
            bestLength = hbuffer+1;
            for(int i = y1 - h0; i < std::min(int(y1 + h0 + 1), hbuffer); i++){
                int currentLength = computeLength(hbuffer, wbuffer, i, 1);
                if((currentLength <= bestLength) && currentLength >= 0){
                    bestLength = currentLength;
                    bestCut1 = i;
                }
            }

            //Trouver les coupes verticales optimales
            int bestCut2 = NULL;
            bestLength = wbuffer+1;
            for(int i = std::max(int(x0 - w0), 0); i < x0 + w0 + 1; i++){
                int currentLength = computeLength(hbuffer, wbuffer, i, 0);
                if((currentLength <= bestLength) && currentLength >= 0){
                    bestLength = currentLength;
                    bestCut2 = i;
                }
            }

            int bestCut3 = NULL;
            bestLength = wbuffer+1;
            for(int i = x1 - w0; i < std::min(int(x1 + w0 + 1), wbuffer); i++){
                int currentLength = computeLength(hbuffer, wbuffer, i, 0);
                if((currentLength <= bestLength) && currentLength >= 0){
                    bestLength = currentLength;
                    bestCut3 = i;
                }
            }

            std::vector<int> cuts;
            cuts.push_back(bestCut0);
            cuts.push_back(bestCut1);
            cuts.push_back(bestCut2);
            cuts.push_back(bestCut3);

            return cuts;
        }

        //Calcule les coupes optimales pour toutes les coupes
        std::vector<std::vector<int>> optimizedCuts(int hbuffer, int wbuffer, std::vector<std::vector<QPoint>> cuts){
            std::vector<std::vector<int>> newCuts;
            for(int i = 0; i < cuts.size(); i++){
                std::vector<QPoint> currentCut = cuts[i];
                newCuts.push_back(optimizeCut(hbuffer, wbuffer, currentCut));
            }
            return newCuts;
        }

        //Cette fonction réalise le packing : elle prend en entrée des rectangles et les arrange
        std::vector<std::vector<int>> packRectangles(std::vector<Chart*> charts, int hbuffer, int wbuffer, bool test=false){
            if(test){
             std::cout<<"charts.size() dans packRectangles: "<<charts.size()<<std::endl;
            }
            std::vector<std::vector<int>> newRectangles;
            std::vector<std::vector<int>> sizes;

            //On récupère l'aire, la longueur et la largeur de chaque rectangle
            for(int i = 0; i < charts.size(); i++){
                Chart * chart = charts[i];
                std::vector<QPoint> pixels = chart->getPixelsInChart();
                std::vector<QPoint> bB = chart->getBoundingBox();
                int x0 = bB[0].x();
                int y0 = bB[0].y();
                int x1 = bB[1].x();
                int y1 = bB[1].y();
                int width = std::abs(x1 - x0);
                int height = std::abs(y1 - y0);
                std::vector<int> size;
                size.push_back(i);
                size.push_back(width*height);
                size.push_back(std::max(width, height));
                size.push_back(std::min(width, height));
                sizes.push_back(size);
            }

            struct {
                    bool operator()(std::vector<int> size1, std::vector<int> size2) const  //opérateur qui compare l'aire de deux rectangles pour les trier
                    {
                        int s1 = size1[1];
                        int s2 = size2[1];

                        return s1 > s2;
                    }
                } areaIsBigger;

            //On range les rectangles par aire croissante, puis par plus grand côté croissant
            std::sort(sizes.begin(), sizes.end(), areaIsBigger);

            //On initialise un arbre avec une taille assez grande (à discuter)
            PackingTree * packingTree = new PackingTree(10*hbuffer, 10*wbuffer);

            //On calcule la nouvelle place de chacun des rectangles grâce à l'arbre
            for(int i = 0; i < sizes.size(); i++){
                std::vector<int> newRectangle = packingTree->place(sizes[i]);
                newRectangles.push_back(newRectangle);
            }
            if(test){
             std::cout<<"newRectangle.size() à la fin de packRectangles: "<<newRectangles.size()<<std::endl;
            }
            return newRectangles;
        }

        //Cette fonction calcule les coordonnées des rectangles nés d'une coupe horizontale
        void packVerticalCut(int x1, int x2, std::vector<Chart*> & charts, bool test = false){ //attention y1 et y2 doivent être les coordonnées de la cut exactes !!
            if(test){
                std::cout<<"Nombre de chart avant le packVerticalCut: "<<charts.size()<<std::endl;
                std::cout<<"x1 dans packVerticalCut: "<<x1<<std::endl;
                std::cout<<"x2 dans packVerticalCut: "<<x2<<std::endl;
            }
            std::vector<QPoint> bB;
            std::vector<Chart*> newCharts;
            for(int i = 0; i < charts.size(); i++){
                bB = charts[i]->getBoundingBox();
                std::vector<Chart*> sons;
                int xmin = bB[0].x();
                int xmax = bB[1].x();
                if(test){
                    std::cout<<"xmin pour la "<<i<<" ème chart: "<<xmin<<std::endl;
                    std::cout<<"xmax pour la "<<i<<" ème chart: "<<xmax<<std::endl;
                }
                if(xmax >= x2 && xmin <= x1){
                    sons = charts[i]->splitChart(x1, x2, 0);
                    if(test){std::cout << "J'ajoute des fils dans la cond1 : " << sons.size() << std::endl;}
                    for(int i = 0; i < sons.size(); i++){
                        newCharts.push_back(sons[i]);
                    }
                }
                else if(xmin <= x1 && x1 <= xmax){
                    sons = charts[i]->splitChart(xmin, x1, 0);
                    if(test){std::cout << "J'ajoute des fils dans la cond2: " << sons.size() << std::endl;}
                    for(int i = 0; i < sons.size(); i++){
                        newCharts.push_back(sons[i]);
                    }
                }
                else if(xmin <= x2 && x2 <= xmax){
                    sons = charts[i]->splitChart(xmin, x2, 0);
                    if(test){std::cout << "J'ajoute des fils dans la cond3: " << sons.size() << std::endl;}
                    for(int i = 0; i < sons.size(); i++){
                        newCharts.push_back(sons[i]);
                    }
                }
                else{
                    newCharts.push_back(charts[i]);
                }
            }
            charts = newCharts;
            if(test){std::cout<<"Nombre de chart après le packVerticalCut: "<<charts.size()<<std::endl;}

        }

        //Cette fonction calcule les coordonnées des rectangles nés d'une coupe verticale
        void packHorizontalCut(int y1, int y2, std::vector<Chart*> & charts, bool test = false){ //attention x1 et x2 doivent être les coordonnées de la cut exactes !!
            if(test){
                std::cout<<"Nombre de chart avant le packHorizontalCut: "<<charts.size()<<std::endl;
                std::cout<<"y1 dans packVerticalCut: "<<y1<<std::endl;
                std::cout<<"y2 dans packVerticalCut: "<<y2<<std::endl;
            }
            std::vector<QPoint> bB;
            std::vector<Chart*> newCharts;
            for(int i = 0; i < charts.size(); i++){
                bB = charts[i]->getBoundingBox();
                std::vector<Chart*> sons;
                int ymin = bB[0].y();
                int ymax = bB[1].y();
                if(test){
                    std::cout<<"ymin pour la "<<i<<" ème chart: "<<ymin<<std::endl;
                    std::cout<<"ymax pour la "<<i<<" ème chart: "<<ymax<<std::endl;
                }
                if(ymax >= y2 && ymin <= y1){
                    sons = charts[i]->splitChart(y1, y2, 1);
                    if(test){std::cout << "J'ajoute des fils dans la cond1 : " << sons.size() << std::endl;}
                    for(int i = 0; i < sons.size(); i++){
                        newCharts.push_back(sons[i]);
                    }
                }
                else if(ymin <= y1 && y1 <= ymax){
                    sons = charts[i]->splitChart(ymin, y1, 1);
                    if(test){std::cout << "J'ajoute des fils dans la cond2 : " << sons.size() << std::endl;}
                    for(int i = 0; i < sons.size(); i++){
                        newCharts.push_back(sons[i]);
                    }
                }
                else if(ymin <= y2 && y2 <= ymax){
                    sons = charts[i]->splitChart(ymin, y2, 1);
                    if(test){std::cout << "J'ajoute des fils dans la cond3 : " << sons.size() << std::endl;}
                    for(int i = 0; i < sons.size(); i++){
                        newCharts.push_back(sons[i]);
                    }
                }
                else{
                    newCharts.push_back(charts[i]);
                }
            }
            charts = newCharts;
            if(test){std::cout<<"charts.size() à la fin de packHorizontalCut: "<<charts.size()<<std::endl;}
        }

        //Après packing, calcul de la taille nécessaire pour la nouvelle image
        std::vector<int> computeImageSize(std::vector<std::vector<int>> rectangles, int hbuffer, int wbuffer){
            int xmax = 0;
            int ymax = 0;
            std::vector<int> newSize;

            for(int i = 0; i < rectangles.size(); i++){
                std::vector<int> rectangle = rectangles[i];
                if(rectangle[3] > xmax){
                    xmax = rectangle[3];
                }
                if(rectangle[4] > ymax){
                    ymax = rectangle[4];
                }
            }
            newSize.push_back(xmax);
            newSize.push_back(ymax);
            return newSize;
        }

        //Réalise toutes les coupes et renvoie l'indice de la coupe qui donne la plus petite image après packing
        std::vector<int> bestCut(std::vector<std::vector<int>> newCuts, int hbuffer, int wbuffer){
            int lowestArea = 750*750;
            int orientation, min, max;
            std::cout << "best cut au pouvoir" << std::endl;
            int bestBoundingBox = 0;
            for(int i = 0; i < newCuts.size(); i++){
                //std::cout<< "Traitement de la coupe : " << i << std::endl;
                std::vector<Chart*> currentChartV = charts;
                //std::cout << "Taille des charts V : " << currentChartV.size() << std::endl;
                std::vector<Chart*> currentChartH = charts;
                //std::cout << "Taille des charts H : " << currentChartH.size() << std::endl;

                std::vector<int> currentCut = newCuts[i];
                packVerticalCut(currentCut[2], currentCut[3], currentChartV);
                //std::cout << "Taille des charts V après: " << currentChartV.size() << std::endl;
                packHorizontalCut(currentCut[0], currentCut[1], currentChartH);
                //std::cout << "Taille des charts H après: " << currentChartH.size() << std::endl;

                std::vector<std::vector<int>> newRectanglesV = packRectangles(currentChartV, hbuffer, wbuffer);
                std::vector<std::vector<int>> newRectanglesH = packRectangles(currentChartH, hbuffer, wbuffer);
                std::vector<int> sizeV = computeImageSize(newRectanglesV, hbuffer, wbuffer);
                int newAreaV = sizeV[0] * sizeV[1];
                //std::cout << "Taille verticale : " << sizeV[0] << "x" << sizeV[1] << std::endl;
                std::vector<int> sizeH = computeImageSize(newRectanglesH, hbuffer, wbuffer);
                int newAreaH = sizeH[0] * sizeH[1];
                //std::cout << "Taille horizontale : " << sizeH[0] << "x" << sizeH[1] << std::endl;
                std::cout<<"newCuts["<<i<<"][0]: "<<newCuts[i][0]<<", newCuts["<<i<<"][1]: "<<newCuts[i][1]<<", newCuts["<<i<<"][2]: "<<newCuts[i][2]<<", newCuts["<<i<<"][3]: "<<newCuts[i][3]<<std::endl;
                if(newAreaV < lowestArea){
                    bestBoundingBox = i;
                    std::cout << "Nouvelle meilleure coupe avec une aire de : " << sizeV[0] << "x" << sizeV[1] << std::endl;
                    lowestArea = newAreaV;
                    orientation = 0;
                    min = newCuts[i][2];
                    max = newCuts[i][3];
                    std::cout<<"min cut: "<<min<<std::endl;
                    std::cout<<"max cut: "<<max<<std::endl;
                }
                if(newAreaH < lowestArea){
                    bestBoundingBox = i;
                    std::cout << "Nouvelle meilleure coupe avec une aire de : " << sizeH[0] << "x" << sizeH[1] << std::endl;
                    lowestArea = newAreaH;
                    orientation = 1;
                    min = newCuts[i][0];
                    max = newCuts[i][1];
                    std::cout<<"min cut courant: "<<min<<std::endl;
                    std::cout<<"max cut courant: "<<max<<std::endl;
                }

            }
            std::cout<<"on choisit la Bounding Box "<<bestBoundingBox<<std::endl;
            std::cout<<"orientation de la cut: "<<orientation<<std::endl;
            std::cout<<"min cut final: "<<min<<std::endl;
            std::cout<<"max cut final: "<<max<<std::endl;
            std::vector<int> bestCut;

            bestCut.push_back(orientation);
            bestCut.push_back(min);
            bestCut.push_back(max);

            return bestCut;
        }

        void testBestCut(int wbuffer, int hbuffer, std::vector<std::vector<QPoint>> emptyBoxes){
            //Test 4
            std::vector<std::vector<int>> newCuts = optimizedCuts(hbuffer, wbuffer, emptyBoxes);
            std::cout << "helloooooooo" << std::endl;

            std::vector<int> bCut = bestCut(newCuts, hbuffer, wbuffer);

            std::cout << "helloooooooo" << std::endl;
            if(bCut[0] == 0){
                packVerticalCut(bCut[1], bCut[2], charts);
            }
            else{
                packHorizontalCut(bCut[1], bCut[2], charts);
            }
            std::cout << "helloooooooo" << std::endl;

            QImage * entree = new QImage(wbuffer, hbuffer,QImage::Format_RGB32);
            entree->fill(QColor(0, 0, 0));
            std::cout << "Il y a : " << charts.size() << "bb à placer" << std::endl;
            for(int i = 0; i < charts.size(); i++){
                Chart * chart = charts[i];

                std::vector<QPoint> bB = chart->getBoundingBox();
                int colorFactor = (int) (((float) i/ (float) charts.size())*255);
                QColor chartColor = QColor(colorFactor,255-colorFactor, 0);


                for(int j = bB[0].x(); j < bB[1].x() ; j++){
                    for(int k = bB[0].y(); k < bB[1].y(); k++){
                        entree->setPixelColor(j, k, chartColor);
                    }
                }
            }
            entree->save("entree.jpg");

            std::vector<std::vector<int>> newRectangles = packRectangles(charts, hbuffer, wbuffer);

            QImage * sortie = new QImage(1500, 1500, QImage::Format_RGB32);
            sortie->fill(QColor(0, 0, 0));
            std::cout << "J'en place : " << newRectangles.size() << std::endl;
            for(int i = 0; i < newRectangles.size(); i++){
                std::vector<int> r = newRectangles[i];
                int colorFactor = (int) (((float) i/ (float) newRectangles.size())*255);
                QColor chartColor = QColor(colorFactor,255-colorFactor, 0);
                //std::cout << "Coord de la new bb : " << r[1] << " " << r[2] << " " << r[3] << " " << r[4] << std::endl;

                for(int j = r[1]; j < r[3] ; j++){
                    for(int k = r[2]; k < r[4]; k++){
                        sortie->setPixelColor(j, k, chartColor);
                    }
                }
            }
            sortie->save("sortie.jpg");
        }

    //calcule les différentes charts de l'atlas
    void computeCharts(int wbuffer, int hbuffer){
        std::cout<<"charts.size(): "<<charts.size()<<std::endl;
        int chartIndex = 0;
        std::cout<<"on commence à calculer les charts"<<std::endl;
        for(int i = 0; i<wbuffer; i++){
            std::vector<bool> examinedPixelsVector = std::vector<bool>();
            std::vector<int> chartOfPixelsVector = std::vector<int>();
            for(int j = 0; j<hbuffer; j++){
                examinedPixelsVector.push_back(false);
                chartOfPixelsVector.push_back(-1);
            }
            examinedPixels.push_back(examinedPixelsVector);
            chartOfPixels.push_back(chartOfPixelsVector);
        }
        for(int x = 0; x<wbuffer; x++){
            for(int y = 0; y<hbuffer; y++){
                if((!examinedPixels[x][y]) && atlas.pixelColor(x,y).red() != 255 && atlas.pixelColor(x,y).green() != 255 && atlas.pixelColor(x,y).blue() != 255){
                    Chart * chart = new Chart();
                    std::cout<<"chart numéro "<< chartIndex << std::endl;
                    examinePixel(wbuffer,hbuffer,x,y,chart, chartIndex);
                    charts.push_back(chart);
                    chartIndex++;
                    std::cout<<"charts.size() après ajout: "<<charts.size()<<std::endl;
                }
            }
        }
        std::cout<<"Nombre de charts: "<<charts.size()<<std::endl;
        QImage chartsImage = QImage(wbuffer,hbuffer, QImage::Format_RGB32);
        chartsImage.fill(QColor(0,0,0));
        for(int i = 0; i<charts.size(); i++){
            int colorFactor = (int) (((float) i/ (float) charts.size())*255);
            QColor chartColor = QColor(colorFactor,255-colorFactor, 0);
            Chart * chart = charts[i];
            std::vector<QPoint> pixelsInChart = chart->getPixelsInChart();
            for(int k = 0; k<pixelsInChart.size(); k++){
                chartsImage.setPixelColor(pixelsInChart[k],chartColor);
            }
        }
        chartsImage.save("chartsImage.jpg");
    }

    //fonction récursive utilisée dans computeChart qui examine un pixel, puis examine ses pixels voisins dans le cas où il est non vide
    void examinePixel(int wbuffer, int hbuffer, int x, int y, Chart * chart, int chartIndex){
        //std::cout<<"début examinePixel"<<std::endl;
        //std::cout<<"   on passe par le pixel ("<< x << "," << y <<")"<<std::endl;
        //std::cout<<"atlas.size: "<<atlas.size().width()<<","<<atlas.size().height()<<std::endl;

        if((x>=0) && (x<wbuffer) && (y>=0) && (y<hbuffer)){
            //std::cout<<"examinedPixels[x][y]: "<<examinedPixels[x][y]<<std::endl;
            if ((!examinedPixels[x][y]) && atlas.pixelColor(x,y).red() != 255 && atlas.pixelColor(x,y).green() != 255 && atlas.pixelColor(x,y).blue() != 255){
                examinedPixels[x][y] = true;
                chart->addPixelInChart(QPoint(x,y));
                chartOfPixels[x][y] = chartIndex;
                //std::cout<<"   on rajoute le pixel ("<< x << "," << y <<")"<<std::endl;
                examinePixel(wbuffer,hbuffer,x-1,y-1,chart,chartIndex);
                examinePixel(wbuffer,hbuffer,x-1,y,chart,chartIndex);
                examinePixel(wbuffer,hbuffer,x-1,y+1,chart,chartIndex);
                examinePixel(wbuffer,hbuffer,x,y-1,chart,chartIndex);
                examinePixel(wbuffer,hbuffer,x,y+1,chart,chartIndex);
                examinePixel(wbuffer,hbuffer,x+1,y-1,chart,chartIndex);
                examinePixel(wbuffer,hbuffer,x+1,y,chart,chartIndex);
                examinePixel(wbuffer,hbuffer,x+1,y+1,chart,chartIndex);

            }
            examinedPixels[x][y] = true;
        }
        if(charts.size()>1){
            std::cout<<"on a fini de passer par ("<< x << "," << y <<")"<<std::endl;
            std::cout<<"charts.size() à la fin de examinePixel: "<<charts.size()<<std::endl;
        }
    }

    void updateAtlas(int wbuffer, int hbuffer, std::vector<std::vector<int>> newChartsBoundingBoxes){
        std::cout<<"newChartsBoundingBoxes.size() au début de updateAtlas: "<<newChartsBoundingBoxes.size()<<std::endl;
        QImage newAtlas = QImage(10*wbuffer,10*hbuffer, QImage::Format_RGB32);
        newAtlas.fill(QColor(255,255,255));
        int xMax = 0;
        int yMax = 0;
        for(int i = 0; i<newChartsBoundingBoxes.size(); i++){
            Chart * chartToMove = charts[newChartsBoundingBoxes[i][0]];
            xMax = std::max(xMax, newChartsBoundingBoxes[i][3]);
            yMax = std::max(yMax, newChartsBoundingBoxes[i][4]);
            std::vector<QPoint> pixelsInChart = chartToMove->getPixelsInChart();
            if(newChartsBoundingBoxes[i][3]-newChartsBoundingBoxes[i][1] == chartToMove->getBoundingBox()[1].x()-chartToMove->getBoundingBox()[0].x()){
                std::cout<<"pas de rotation"<<std::endl;
                int moveX = newChartsBoundingBoxes[i][1]-chartToMove->getBoundingBox()[0].x();
                int moveY = newChartsBoundingBoxes[i][2]-chartToMove->getBoundingBox()[0].y();
                for(int k = 0; k<pixelsInChart.size(); k++){
                    newAtlas.setPixelColor(pixelsInChart[k].x()+moveX, pixelsInChart[k].y()+moveY, QColor(0,255,0));
                }
            }
            else{
                std::cout<<"rotation"<<std::endl;
                int moveX = newChartsBoundingBoxes[i][1]-chartToMove->getBoundingBox()[0].y();
                int moveY = newChartsBoundingBoxes[i][2]-chartToMove->getBoundingBox()[0].x();
                for(int k = 0; k<pixelsInChart.size(); k++){
                    newAtlas.setPixelColor(pixelsInChart[k].y()+moveX, pixelsInChart[k].x()+moveY, QColor(0,255,0));
                }
            }
        }
        atlas = QImage(xMax+1,yMax+1,QImage::Format_RGB32);
        for(int x = 0; x<xMax+1; x++){
            for(int y = 0; y<yMax; y++){
                atlas.setPixelColor(x,y,newAtlas.pixelColor(x,y));
            }
        }
    }


    //met à jour les coordonnées uv et les triangles de l'atlas après avoir trouvé un packing
    void updateMeshText(int wbuffer, int hbuffer, std::vector<std::vector<int>> newChartsBoundingBoxes, int directionOfCut, int minCutPixel, int maxCutPixel){
        computeMeshTextCharts();
        cutMeshText(wbuffer, hbuffer, directionOfCut, minCutPixel);
        cutMeshText(wbuffer, hbuffer, directionOfCut, maxCutPixel);
        linkChartsRepresentations(wbuffer, hbuffer);
        moveCharts(wbuffer, hbuffer, newChartsBoundingBoxes);

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
        for(int k = 0; k<mesh.chartsTriangles.size();k++){
            std::cout<<k<<" ème chart étudiée dans cutMeshText"<<std::endl;
            std::vector<Triangle> chartsLeft = std::vector<Triangle>();
            std::vector<Triangle> chartsRight = std::vector<Triangle>();
            for(int i = 0; i<mesh.chartsTriangles[k].size(); i++){
                int corner0 = mesh.chartsTriangles[k][i][0];
                int corner1 = mesh.chartsTriangles[k][i][1];
                int corner2 = mesh.chartsTriangles[k][i][2];

                double pos0 = mesh.textcoords[corner0][directionOfCut];
                double pos1 = mesh.textcoords[corner1][directionOfCut];
                double pos2 = mesh.textcoords[corner2][directionOfCut];

                int numberOfNewVertices = 0;
                std::vector<int> oppositeVertices = std::vector<int>(); //stocke les sommets opposés des segments coupés
                if(pos0 < cutPosition && pos1 < cutPosition && pos2 < cutPosition){
                    chartsLeft.push_back(mesh.chartsTriangles[k][i]);
                }
                else if(pos0 > cutPosition && pos1 > cutPosition && pos2 > cutPosition){
                    chartsRight.push_back(mesh.chartsTriangles[k][i]);
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
                        tri0.corners[1] = mesh.textcoords.size()-3;
                        tri0.corners[2] = mesh.textcoords.size()-1;

                        tri1.corners[0] = mesh.textcoords.size()-4;
                        tri1.corners[1] = corner1;
                        tri1.corners[2] = mesh.textcoords.size()-2;

                        tri2.corners[0] = mesh.textcoords.size()-2;
                        tri2.corners[1] = corner1;
                        tri2.corners[2] = corner2;
                        if(pos0<cutPosition){
                            chartsLeft.push_back(tri0);
                            chartsRight.push_back(tri1);
                            chartsRight.push_back(tri2);

                        }
                        else{
                            chartsRight.push_back(tri0);
                            chartsLeft.push_back(tri1);
                            chartsLeft.push_back(tri2);

                        }


                    }
                    else if(oppositeVertices[0] == 2 && oppositeVertices[1] == 0){
                        tri0.corners[0] = corner1;
                        tri0.corners[1] = mesh.textcoords.size()-1;
                        tri0.corners[2] = mesh.textcoords.size()-3;

                        tri1.corners[0] = mesh.textcoords.size()-4;
                        tri1.corners[1] = mesh.textcoords.size()-2;
                        tri1.corners[2] = corner2;

                        tri2.corners[0] = mesh.textcoords.size()-4;
                        tri2.corners[1] = corner2;
                        tri2.corners[2] = corner0;
                        if(pos1<cutPosition){
                            chartsLeft.push_back(tri0);
                            chartsRight.push_back(tri1);
                            chartsRight.push_back(tri2);

                        }
                        else{
                            chartsRight.push_back(tri0);
                            chartsLeft.push_back(tri1);
                            chartsLeft.push_back(tri2);

                        }
                    }
                    else if(oppositeVertices[0] == 1 && oppositeVertices[1] == 0){
                        tri0.corners[0] = corner2;
                        tri0.corners[1] = mesh.textcoords.size()-3;
                        tri0.corners[2] = mesh.textcoords.size()-1;

                        tri1.corners[0] = mesh.textcoords.size()-2;
                        tri1.corners[1] = mesh.textcoords.size()-4;
                        tri1.corners[2] = corner0;

                        tri2.corners[0] = mesh.textcoords.size()-2;
                        tri2.corners[1] = corner0;
                        tri2.corners[2] = corner1;
                        if(pos2<cutPosition){
                            chartsLeft.push_back(tri0);
                            chartsRight.push_back(tri1);
                            chartsRight.push_back(tri2);
                        }
                        else{
                            chartsRight.push_back(tri0);
                            chartsLeft.push_back(tri1);
                            chartsLeft.push_back(tri2);
                        }
                    }
                }
            }
            if(chartsLeft.size() ==  0 || chartsRight.size() == 0){
                std::cout<<"on remet l'ancien chart"<<std::endl;
                newChartsTriangles.push_back(mesh.chartsTriangles[k]);
            }
            else{
                std::cout<<"le chart est coupé en deux"<<std::endl;
                newChartsTriangles.push_back(chartsLeft);
                newChartsTriangles.push_back(chartsRight);
            }
        }
        std::cout<<"taille newChartsTriangles à la fin du cut: "<<newChartsTriangles.size()<<std::endl;
        mesh.chartsTriangles = newChartsTriangles;
        mesh.updateTextTriangles();
    }

    //crée deux fois le même vertex: une occurence pour chaque chart
    void createNewVertexTwice(int wbuffer, int hbuffer, int cornerA, int cornerB, int directionOfCut, double cutPosition){
        double tA = mesh.textcoords[cornerA][directionOfCut];
        double zA = mesh.textcoords[cornerA][1-directionOfCut];
        double tB = mesh.textcoords[cornerB][directionOfCut];
        double zB = mesh.textcoords[cornerB][1-directionOfCut];

        double coeff = (zB-zA)/(tB-tA);
        switch(directionOfCut){
            case 0: mesh.textcoords.push_back(uvCoord(cutPosition,coeff*(cutPosition-tA)+zA));
                    mesh.textcoords.push_back(uvCoord(cutPosition,coeff*(cutPosition-tA)+zA));
                    break;
            case 1: mesh.textcoords.push_back(uvCoord(coeff*(cutPosition-tA)+zA,cutPosition));
                    mesh.textcoords.push_back(uvCoord(coeff*(cutPosition-tA)+zA,cutPosition));
                    break;
        }

    }

    void computeMeshTextCharts(){
        mesh.updateNeighbours();
        mesh.chartOfVertex = std::vector<int>();
        mesh.charts = std::vector<std::vector<int>>();
        mesh.chartsBoundingBoxes = std::vector<std::vector<double>>();
        for(int k = 0; k<mesh.textcoords.size(); k++){
            mesh.chartOfVertex.push_back(-1);
        }
        for(int k = 0; k<mesh.textcoords.size(); k++){
            if(mesh.chartOfVertex[k] == -1){
                std::vector<int> chart = std::vector<int>();
                std::vector<Triangle> chartTriangles = std::vector<Triangle>();
                std::vector<double> boundingBox = std::vector<double>();
                boundingBox.push_back(mesh.textcoords[k][0]);
                boundingBox.push_back(mesh.textcoords[k][1]);
                boundingBox.push_back(mesh.textcoords[k][0]);
                boundingBox.push_back(mesh.textcoords[k][1]);
                mesh.charts.push_back(chart);
                mesh.chartsTriangles.push_back(chartTriangles);
                mesh.chartsBoundingBoxes.push_back(boundingBox);

                examineVertex(k);
            }
        }
        std::cout<<"nombre de charts du mesh: "<<mesh.charts.size()<<std::endl;

        for(int i = 0; i<mesh.triangles_text.size(); i++){
            mesh.chartsTriangles[mesh.chartOfVertex[mesh.triangles_text[i][0]]].push_back(mesh.triangles_text[i]);
        }
        std::cout<<"fin de computeMeshTextCharts"<<std::endl;
    }

    void examineVertex(int k){
        mesh.charts[mesh.charts.size()-1].push_back(k);
        mesh.chartOfVertex[k] = mesh.charts.size()-1;
        mesh.chartsBoundingBoxes[mesh.charts.size()-1][0] = std::min(mesh.textcoords[k][0],mesh.chartsBoundingBoxes[mesh.charts.size()-1][0]);
        mesh.chartsBoundingBoxes[mesh.charts.size()-1][1] = std::min(mesh.textcoords[k][1],mesh.chartsBoundingBoxes[mesh.charts.size()-1][1]);
        mesh.chartsBoundingBoxes[mesh.charts.size()-1][2] = std::max(mesh.textcoords[k][0],mesh.chartsBoundingBoxes[mesh.charts.size()-1][2]);
        mesh.chartsBoundingBoxes[mesh.charts.size()-1][3] = std::max(mesh.textcoords[k][0],mesh.chartsBoundingBoxes[mesh.charts.size()-1][3]);
        for(int i = 0; i<mesh.neighbours[k].size(); i++){
            if(mesh.chartOfVertex[mesh.neighbours[k][i]]== -1){
                examineVertex(mesh.neighbours[k][i]);
            }
        }
    }

    /*void computeMeshTextCharts(){
      int chartOfVertex[mesh.textcoords.size()];
      for(int i = 0; i<mesh.textcoords.size(); i++){
          std::vector<int> chart = std::vector<int>();
          std::vector<double> boundingBox = std::vector<double>();
          chartOfVertex[i] = -1;
      }
      mesh.charts = std::vector<std::vector<int>>();
      mesh.chartsBoundingBoxes = std::vector<std::vector<double>>();
      for(int i = 0; i<mesh.triangles_text.size(); i++){
          std::cout<<i<<"ème passage dans la boucle de computeMeshTextCharts()"<<std::endl;
          int corner0 = mesh.triangles_text[i][0];
          int corner1 = mesh.triangles_text[i][1];
          int corner2 = mesh.triangles_text[i][2];

          if(chartOfVertex[corner0] != -1){
              std::cout<<"condition 1"<<std::endl;
              mesh.chartsTriangles[chartOfVertex[corner0]].push_back(mesh.triangles_text[i]);
              mesh.charts[chartOfVertex[corner0]].push_back(corner1);
              mesh.charts[chartOfVertex[corner0]].push_back(corner2);
              chartOfVertex[corner1] = chartOfVertex[corner0];
              chartOfVertex[corner2] = chartOfVertex[corner0];
              mesh.chartsBoundingBoxes[chartOfVertex[corner0]][0] = std::min(std::min(mesh.textcoords[corner1][0],mesh.textcoords[corner2][0]), mesh.chartsBoundingBoxes[chartOfVertex[corner0]][0]);
              mesh.chartsBoundingBoxes[chartOfVertex[corner0]][1] = std::max(std::max(mesh.textcoords[corner1][0],mesh.textcoords[corner2][0]), mesh.chartsBoundingBoxes[chartOfVertex[corner0]][1]);
              mesh.chartsBoundingBoxes[chartOfVertex[corner0]][2] = std::min(std::min(mesh.textcoords[corner1][1],mesh.textcoords[corner2][1]), mesh.chartsBoundingBoxes[chartOfVertex[corner0]][2]);
              mesh.chartsBoundingBoxes[chartOfVertex[corner0]][3] = std::max(std::max(mesh.textcoords[corner1][1],mesh.textcoords[corner2][1]), mesh.chartsBoundingBoxes[chartOfVertex[corner0]][3]);
          }
          else if(chartOfVertex[corner1] != -1){
              std::cout<<"condition 2"<<std::endl;
              mesh.chartsTriangles[chartOfVertex[corner1]].push_back(mesh.triangles_text[i]);
              mesh.charts[chartOfVertex[corner1]].push_back(corner0);
              mesh.charts[chartOfVertex[corner1]].push_back(corner2);
              chartOfVertex[corner0] = chartOfVertex[corner1];
              chartOfVertex[corner2] = chartOfVertex[corner1];
              mesh.chartsBoundingBoxes[chartOfVertex[corner1]][0] = std::min(std::min(mesh.textcoords[corner0][0],mesh.textcoords[corner2][0]), mesh.chartsBoundingBoxes[chartOfVertex[corner1]][0]);
              mesh.chartsBoundingBoxes[chartOfVertex[corner1]][1] = std::max(std::max(mesh.textcoords[corner0][0],mesh.textcoords[corner2][0]), mesh.chartsBoundingBoxes[chartOfVertex[corner1]][1]);
              mesh.chartsBoundingBoxes[chartOfVertex[corner1]][2] = std::min(std::min(mesh.textcoords[corner0][1],mesh.textcoords[corner2][1]), mesh.chartsBoundingBoxes[chartOfVertex[corner1]][2]);
              mesh.chartsBoundingBoxes[chartOfVertex[corner1]][3] = std::max(std::max(mesh.textcoords[corner0][1],mesh.textcoords[corner2][1]), mesh.chartsBoundingBoxes[chartOfVertex[corner1]][3]);
          }
          else if(chartOfVertex[corner2] != -1){
              std::cout<<"condition 3"<<std::endl;
              mesh.chartsTriangles[chartOfVertex[corner2]].push_back(mesh.triangles_text[i]);
              std::cout<<"line 909"<<std::endl;
              mesh.charts[chartOfVertex[corner2]].push_back(corner0);
              mesh.charts[chartOfVertex[corner2]].push_back(corner1);
              chartOfVertex[corner0] = chartOfVertex[corner2];
              chartOfVertex[corner1] = chartOfVertex[corner2];
              std::cout<<"line 914"<<std::endl;
              mesh.chartsBoundingBoxes[chartOfVertex[corner2]][0] = std::min(std::min(mesh.textcoords[corner0][0],mesh.textcoords[corner1][0]), mesh.chartsBoundingBoxes[chartOfVertex[corner0]][0]);
              mesh.chartsBoundingBoxes[chartOfVertex[corner2]][1] = std::max(std::max(mesh.textcoords[corner0][0],mesh.textcoords[corner1][0]), mesh.chartsBoundingBoxes[chartOfVertex[corner0]][1]);
              mesh.chartsBoundingBoxes[chartOfVertex[corner2]][2] = std::min(std::min(mesh.textcoords[corner0][1],mesh.textcoords[corner1][1]), mesh.chartsBoundingBoxes[chartOfVertex[corner0]][2]);
              mesh.chartsBoundingBoxes[chartOfVertex[corner2]][3] = std::max(std::max(mesh.textcoords[corner0][1],mesh.textcoords[corner1][1]), mesh.chartsBoundingBoxes[chartOfVertex[corner0]][3]);
          }
          else{
              std::cout<<"condition 4"<<std::endl;
              std::vector<int> chart = std::vector<int>();
              std::vector<Triangle> chartTriangles = std::vector<Triangle>();
              std::vector<double> boundingBox = std::vector<double>();
              chartTriangles.push_back(mesh.triangles_text[i]);
              mesh.chartsTriangles.push_back(chartTriangles);
              chart.push_back(corner0);
              chart.push_back(corner1);
              chart.push_back(corner2);
              chartOfVertex[corner0] = mesh.charts.size();
              chartOfVertex[corner1] = mesh.charts.size();
              chartOfVertex[corner2] = mesh.charts.size();
              boundingBox.push_back(std::min(std::min(mesh.textcoords[corner0][0],mesh.textcoords[corner1][0]),mesh.textcoords[corner2][0]));
              boundingBox.push_back(std::max(std::max(mesh.textcoords[corner0][0],mesh.textcoords[corner1][0]),mesh.textcoords[corner2][0]));
              boundingBox.push_back(std::min(std::min(mesh.textcoords[corner0][1],mesh.textcoords[corner1][1]),mesh.textcoords[corner2][1]));
              boundingBox.push_back(std::max(std::max(mesh.textcoords[corner0][1],mesh.textcoords[corner1][1]),mesh.textcoords[corner2][1]));
              mesh.charts.push_back(chart);
              mesh.chartsBoundingBoxes.push_back(boundingBox);
          }
      }
      std::cout<<"nombre de charts du mesh: "<<mesh.charts.size()<<std::endl;
   }*/

   //associe à chaque chart de l'espace UV son chart dans l'espace rasterisé
   void linkChartsRepresentations(int wbuffer, int hbuffer){
       /*for(int k = 0; k<charts.size();k++){
           std::vector<QPoint> boundingBox = std::vector<QPoint>();
           boundingBox.push_back(QPoint((int) (500 * mesh.chartsBoundingBoxes[0][0]), (int) (500 * mesh.chartsBoundingBoxes[0][1])));
           boundingBox.push_back(QPoint((int) (500 * mesh.chartsBoundingBoxes[0][2]), (int) (500 * mesh.chartsBoundingBoxes[0][3])));
           std::cout<<"bounding box de la chart "<<0<<": ("<<(int) (500 * mesh.chartsBoundingBoxes[0][0])<<", "<<(int) (500 * mesh.chartsBoundingBoxes[0][1])<<"); ("<<(int) (500 * mesh.chartsBoundingBoxes[0][2])<<", "<<(int) (500 * mesh.chartsBoundingBoxes[0][3])<<")"<<std::endl;
           int closestChart = 0;
           float overlapMax = overlapAmount(boundingBox, charts[k]->getBoundingBox());
           for(int i = 1; i<mesh.charts.size();i++){
               std::vector<QPoint> boundingBox = std::vector<QPoint>();
               boundingBox.push_back(QPoint((int) (500 * mesh.chartsBoundingBoxes[i][0]), (int) (500 * mesh.chartsBoundingBoxes[i][1])));
               boundingBox.push_back(QPoint((int) (500 * mesh.chartsBoundingBoxes[i][2]), (int) (500 * mesh.chartsBoundingBoxes[i][3])));
               std::cout<<"bounding box de la chart "<<i<<": ("<<(int) (500 * mesh.chartsBoundingBoxes[i][0])<<", "<<(int) (500 * mesh.chartsBoundingBoxes[i][1])<<"); ("<<(int) (500 * mesh.chartsBoundingBoxes[i][2])<<", "<<(int) (500 * mesh.chartsBoundingBoxes[i][3])<<")"<<std::endl;
               float overlap = overlapAmount(boundingBox, charts[k]->getBoundingBox());
               if(overlap>overlapMax){
                   closestChart = i;
                   overlapMax = overlap;
               }
           }
           charts[k]->setMeshTextChartIndex(closestChart);
           std::cout<<"closestChart: "<<closestChart<<std::endl;
       }*/


       for(int i = 0; i<mesh.charts.size();i++){
           std::vector<QPoint> boundingBox = std::vector<QPoint>();
           boundingBox.push_back(QPoint((int) (wbuffer * mesh.chartsBoundingBoxes[i][0]), (int) (hbuffer * mesh.chartsBoundingBoxes[i][1])));
           boundingBox.push_back(QPoint((int) (wbuffer * mesh.chartsBoundingBoxes[i][2]), (int) (hbuffer * mesh.chartsBoundingBoxes[i][3])));
           std::cout<<"bounding box de la chart "<<i<<": ("<<(int) (wbuffer * mesh.chartsBoundingBoxes[i][0])<<", "<<(int) (hbuffer * mesh.chartsBoundingBoxes[i][1])<<"); ("<<(int) (wbuffer * mesh.chartsBoundingBoxes[i][2])<<", "<<(int) (hbuffer * mesh.chartsBoundingBoxes[i][3])<<")"<<std::endl;
           int closestChart = 0;
           float overlapMax = overlapAmount(boundingBox, charts[0]->getBoundingBox());
           for(int k = 1; k<charts.size();k++){
               float overlap = overlapAmount(boundingBox, charts[k]->getBoundingBox());
               if(overlap>overlapMax){
                   closestChart = k;
                   overlapMax = overlap;
               }
           }
           charts[closestChart]->addMeshTextChartIndices(i);
           std::cout<<"closestChart: "<<closestChart<<std::endl;
       }
   }

   void moveCharts(int wbuffer, int hbuffer, std::vector<std::vector<int>> newChartsBoundingBoxes){
       for(int i = 0; i<newChartsBoundingBoxes.size(); i++){
           Chart * chartToMove = charts[newChartsBoundingBoxes[i][0]];
           //int meshTextChartIndex = chartToMove->getMeshTextChartIndex();
           std::vector<int> meshTextChartIndices = chartToMove->getMeshTextChartIndices();
           for(int k = 0; k< meshTextChartIndices.size(); k++){
               int meshTextChartIndex = meshTextChartIndices[k];
               if(newChartsBoundingBoxes[i][3]-newChartsBoundingBoxes[i][1] == chartToMove->getBoundingBox()[1].x()-chartToMove->getBoundingBox()[0].x()){
                   std::cout<<"pas de rotation"<<std::endl;
                   int moveX = newChartsBoundingBoxes[i][1]-chartToMove->getBoundingBox()[0].x();
                   std::cout<<"line 1016 moveCharts"<<std::endl;
                   int moveY = newChartsBoundingBoxes[i][2]-chartToMove->getBoundingBox()[0].y();
                   std::cout<<"line 1018 moveCharts"<<std::endl;
                   mesh.moveVertices(meshTextChartIndex, (double)moveX/(double)wbuffer, (double)moveY/(double) hbuffer);
                   std::cout<<"line 1020 moveCharts"<<std::endl;
               }
               else{
                   std::cout<<"rotation"<<std::endl;
                   int moveX = newChartsBoundingBoxes[i][1]-chartToMove->getBoundingBox()[0].y();
                   int moveY = newChartsBoundingBoxes[i][2]-chartToMove->getBoundingBox()[0].x();
                   mesh.invertXY(meshTextChartIndex);
                   mesh.moveVertices(meshTextChartIndex, (double)moveX/ (double) wbuffer, (double)moveY/(double)hbuffer);
               }
           }
       }
   }



    MyViewer(QGLWidget * parent = NULL) : QGLViewer(parent) , QOpenGLFunctions_3_0() {
    }



    void add_actions_to_toolBar(QToolBar *toolBar)
    {
        // Specify the actions :
        DetailedAction * open_mesh = new DetailedAction( QIcon(":/icons/open.png") , "Open Mesh" , "Open Mesh" , this , this , SLOT(open_mesh()) );
        DetailedAction * save_mesh = new DetailedAction( QIcon(":/icons/save.png") , "Save model" , "Save model" , this , this , SLOT(save_mesh()) );
        DetailedAction * help = new DetailedAction( QIcon(":/icons/help.png") , "HELP" , "HELP" , this , this , SLOT(help()) );
        DetailedAction * saveCamera = new DetailedAction( QIcon(":/icons/camera.png") , "Save camera" , "Save camera" , this , this , SLOT(saveCamera()) );
        DetailedAction * openCamera = new DetailedAction( QIcon(":/icons/open_camera.png") , "Open camera" , "Open camera" , this , this , SLOT(openCamera()) );
        DetailedAction * saveSnapShotPlusPlus = new DetailedAction( QIcon(":/icons/save_snapshot.png") , "Save snapshot" , "Save snapshot" , this , this , SLOT(saveSnapShotPlusPlus()) );

        // Add them :
        toolBar->addAction( open_mesh );
        toolBar->addAction( save_mesh );
        toolBar->addAction( help );
        toolBar->addAction( saveCamera );
        toolBar->addAction( openCamera );
        toolBar->addAction( saveSnapShotPlusPlus );
    }


    void draw() {
        glEnable(GL_DEPTH_TEST);
        glEnable( GL_LIGHTING );
        glColor3f(0.5,0.5,0.8);
        glBegin(GL_TRIANGLES);
        for( unsigned int t = 0 ; t < mesh.triangles.size() ; ++t ) {
            point3d const & p0 = mesh.vertices[ mesh.triangles[t][0] ].p;
            point3d const & p1 = mesh.vertices[ mesh.triangles[t][1] ].p;
            point3d const & p2 = mesh.vertices[ mesh.triangles[t][2] ].p;
            point3d const & n = point3d::cross( p1-p0 , p2-p0 ).direction();
            glNormal3f(n[0],n[1],n[2]);
            glVertex3f(p0[0],p0[1],p0[2]);
            glVertex3f(p1[0],p1[1],p1[2]);
            glVertex3f(p2[0],p2[1],p2[2]);
        }
        glEnd();
    }

    void pickBackgroundColor() {
        QColor _bc = QColorDialog::getColor( this->backgroundColor(), this);
        if( _bc.isValid() ) {
            this->setBackgroundColor( _bc );
            this->update();
        }
    }

    void adjustCamera( point3d const & bb , point3d const & BB ) {
        point3d const & center = ( bb + BB )/2.f;
        setSceneCenter( qglviewer::Vec( center[0] , center[1] , center[2] ) );
        setSceneRadius( 1.5f * ( BB - bb ).norm() );
        showEntireScene();
    }


    void init() {
        makeCurrent();
        initializeOpenGLFunctions();

        setMouseTracking(true);// Needed for MouseGrabber.

        setBackgroundColor(QColor(255,255,255));

        // Lights:
        GLTools::initLights();
        GLTools::setSunsetLight();
        GLTools::setDefaultMaterial();

        //
        glShadeModel(GL_SMOOTH);
        glFrontFace(GL_CCW); // CCW ou CW

        glEnable(GL_DEPTH);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_CLIP_PLANE0);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_COLOR_MATERIAL);

        //
        setSceneCenter( qglviewer::Vec( 0 , 0 , 0 ) );
        setSceneRadius( 10.f );
        showEntireScene();
    }

    QString helpString() const {
        QString text("<h2>Our cool project</h2>");
        text += "<p>";
        text += "This is a research application, it can explode.";
        text += "<h3>Participants</h3>";
        text += "<ul>";
        text += "<li>jmt</li>";
        text += "<li>...</li>";
        text += "</ul>";
        text += "<h3>Basics</h3>";
        text += "<p>";
        text += "<ul>";
        text += "<li>H   :   make this help appear</li>";
        text += "<li>Ctrl + mouse right button double click   :   choose background color</li>";
        text += "<li>Ctrl + T   :   change window title</li>";
        text += "</ul>";
        return text;
    }

    void updateTitle( QString text ) {
        this->setWindowTitle( text );
        emit windowTitleUpdated(text);
    }

    void keyPressEvent( QKeyEvent * event ) {
        if( event->key() == Qt::Key_H ) {
            help();
        }
        else if( event->key() == Qt::Key_T ) {
            if( event->modifiers() & Qt::CTRL )
            {
                bool ok;
                QString text = QInputDialog::getText(this, tr(""), tr("title:"), QLineEdit::Normal,this->windowTitle(), &ok);
                if (ok && !text.isEmpty())
                {
                    updateTitle(text);
                }
            }
        }
    }

    void mouseDoubleClickEvent( QMouseEvent * e )
    {
        if( (e->modifiers() & Qt::ControlModifier)  &&  (e->button() == Qt::RightButton) )
        {
            pickBackgroundColor();
            return;
        }

        if( (e->modifiers() & Qt::ControlModifier)  &&  (e->button() == Qt::LeftButton) )
        {
            showControls();
            return;
        }


        QGLViewer::mouseDoubleClickEvent( e );
    }

    void mousePressEvent(QMouseEvent* e ) {
        QGLViewer::mousePressEvent(e);
    }

    void mouseMoveEvent(QMouseEvent* e  ){
        QGLViewer::mouseMoveEvent(e);
    }

    void mouseReleaseEvent(QMouseEvent* e  ) {
        QGLViewer::mouseReleaseEvent(e);
    }

signals:
    void windowTitleUpdated( const QString & );

public slots:
    void open_mesh() {
        bool success = false;
        QString fileName = QFileDialog::getOpenFileName(NULL,"","");
        if ( !fileName.isNull() ) { // got a file name
            if(fileName.endsWith(QString(".off")))
                success = OFFIO::openTriMesh(fileName.toStdString() , mesh.vertices , mesh.triangles );
            else if(fileName.endsWith(QString(".obj")))
                success = OBJIO::openTriMesh(fileName.toStdString() , mesh.vertices , mesh.triangles , mesh.textcoords , mesh.triangles_text );
            if(success) {
                std::cout << fileName.toStdString() << " was opened successfully" << std::endl;

                // test:
                //computeMeshTextCharts();
                //cutMeshText(500,500,0,250);
                {
                    mesh.triangles = mesh.triangles_text;
                    for( unsigned int v = 0 ; v < mesh.textcoords.size() ; ++v ) {
                        mesh.vertices[v].p = point3d( mesh.textcoords[v][0] , mesh.textcoords[v][1] , 0.0 );
                    }
               }
                renderGeometry(500, 500, 1000);
                computeCharts(500,500);
                for(int i = 0; i<10; i++){
                    int w = atlas.size().width();
                    int h = atlas.size().height();
                    std::cout<<"w: "<<w<<std::endl;
                    std::cout<<"h: "<<h<<std::endl;
                    std::vector<std::vector<QPoint>> emptyBoxes = locateVoidBoxes(w,h,1000);
                    voidBoxesImage.save("voidBoxes"+QString::number(i)+".jpg");
                    //computeCharts(width,height);
                    for(int i = 0; i<charts.size(); i++){
                        std::cout<<"chart "<<i<<std::endl;
                        std::vector<QPoint> boundingBox = charts[i]->getBoundingBox();
                        std::cout<<"minX: "<<boundingBox[0].x()<<", minY: "<<boundingBox[0].y()<<std::endl;
                        std::cout<<"maxY: "<<boundingBox[1].x()<<", maxY: "<<boundingBox[1].y()<<std::endl;
                    }
                    std::vector<std::vector<int>> newCuts = optimizedCuts(h, w, emptyBoxes);
                    std::vector<int> bCut = bestCut(newCuts, h, w);
                    if(bCut[0] == 0){
                        packVerticalCut(bCut[1], bCut[2], charts, true);
                    }
                    else{
                        packHorizontalCut(bCut[1], bCut[2], charts, true);
                    }
                    std::vector<std::vector<int>> newRectangles = packRectangles(charts, h, w, true);
                    updateAtlas(w, h, newRectangles);
                    atlas.save("packedAtlas"+QString::number(i)+".jpg");
                    //updateMeshText(w,h,newRectangles,bCut[0],bCut[1],bCut[2]);
                }
                {
                    mesh.triangles = mesh.triangles_text;
                    for( unsigned int v = 0 ; v < mesh.textcoords.size() ; ++v ) {
                        mesh.vertices[v].p = point3d( mesh.textcoords[v][0] , mesh.textcoords[v][1] , 0.0 );
                    }
                }

                point3d bb(FLT_MAX,FLT_MAX,FLT_MAX) , BB(-FLT_MAX,-FLT_MAX,-FLT_MAX);
                for( unsigned int v = 0 ; v < mesh.vertices.size() ; ++v ) {
                    bb = point3d::min(bb , mesh.vertices[v]);
                    BB = point3d::max(BB , mesh.vertices[v]);
                }
                adjustCamera(bb,BB);
                update();
            }
            else
                std::cout << fileName.toStdString() << " could not be opened" << std::endl;
        }
    }

    void save_mesh() {
        bool success = false;
        QString fileName = QFileDialog::getOpenFileName(NULL,"","");
        if ( !fileName.isNull() ) { // got a file name
            if(fileName.endsWith(QString(".off")))
                success = OFFIO::save(fileName.toStdString() , mesh.vertices , mesh.triangles );
            else if(fileName.endsWith(QString(".obj")))
                success = OBJIO::save(fileName.toStdString() , mesh.vertices , mesh.triangles );
            if(success)
                std::cout << fileName.toStdString() << " was saved" << std::endl;
            else
                std::cout << fileName.toStdString() << " could not be saved" << std::endl;
        }
    }

    void showControls()
    {
        // Show controls :
        controls->close();
        controls->show();
    }

    void saveCameraInFile(const QString &filename){
        std::ofstream out (filename.toUtf8());
        if (!out)
            exit (EXIT_FAILURE);
        // << operator for point3 causes linking problem on windows
        out << camera()->position()[0] << " \t" << camera()->position()[1] << " \t" << camera()->position()[2] << " \t" " " <<
                                          camera()->viewDirection()[0] << " \t" << camera()->viewDirection()[1] << " \t" << camera()->viewDirection()[2] << " \t" << " " <<
                                          camera()->upVector()[0] << " \t" << camera()->upVector()[1] << " \t" <<camera()->upVector()[2] << " \t" <<" " <<
                                          camera()->fieldOfView();
        out << std::endl;

        out.close ();
    }

    void openCameraFromFile(const QString &filename){

        std::ifstream file;
        file.open(filename.toStdString().c_str());

        qglviewer::Vec pos;
        qglviewer::Vec view;
        qglviewer::Vec up;
        float fov;

        file >> (pos[0]) >> (pos[1]) >> (pos[2]) >>
                                                    (view[0]) >> (view[1]) >> (view[2]) >>
                                                                                           (up[0]) >> (up[1]) >> (up[2]) >>
                                                                                                                            fov;

        camera()->setPosition(pos);
        camera()->setViewDirection(view);
        camera()->setUpVector(up);
        camera()->setFieldOfView(fov);

        camera()->computeModelViewMatrix();
        camera()->computeProjectionMatrix();

        update();
    }


    void openCamera(){
        QString fileName = QFileDialog::getOpenFileName(NULL,"","*.cam");
        if ( !fileName.isNull() ) {                 // got a file name
            openCameraFromFile(fileName);
        }
    }
    void saveCamera(){
        QString fileName = QFileDialog::getSaveFileName(NULL,"","*.cam");
        if ( !fileName.isNull() ) {                 // got a file name
            saveCameraInFile(fileName);
        }
    }

    void saveSnapShotPlusPlus(){
        QString fileName = QFileDialog::getSaveFileName(NULL,"*.png","");
        if ( !fileName.isNull() ) {                 // got a file name
            setSnapshotFormat("PNG");
            setSnapshotQuality(100);
            saveSnapshot( fileName );
            saveCameraInFile( fileName+QString(".cam") );
        }
    }

};




#endif // MYVIEWER_H
