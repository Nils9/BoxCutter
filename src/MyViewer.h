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
#include "gl/BasicColors.h"

class MyViewer : public QGLViewer , public QOpenGLFunctions_3_0
{
    Q_OBJECT

    Mesh mesh;

    Mesh copyMesh;

    bool uvMode = false;

    QWidget * controls;

    QImage atlas;

    QImage voidBoxesImage;

    std::vector<std::vector<bool>> examinedPixels = std::vector<std::vector<bool>>();

    std::vector<std::vector<int>> chartOfPixels = std::vector<std::vector<int>>();

    GLuint texture;

    int iteration = 0;

public :

    void boxCutter(){
        iteration++;
        if(!uvMode){
         meshToUV();
        }
        int w = (int)(700*mesh.getWidth());
        int h = (int)(700*mesh.getHeight());
        renderGeometry(w, h);
        QSize newSize = resizeAtlas();
        w = newSize.width();
        h = newSize.height();
        mesh = copyMesh;
        std::cout<<"w: "<<w<<std::endl;
        std::cout<<"h: "<<h<<std::endl;
        std::vector<std::vector<QPoint>> emptyBoxes = locateVoidBoxes(w,h,300);
        voidBoxesImage.save("voidBoxesBis"+QString::number(iteration)+".jpg");
        std::vector<std::vector<int>> newCuts = optimizedCuts(h, w, emptyBoxes);
        std::cout << "Nombre de cuts à analyser : " << newCuts.size() << " x2" << std::endl;
        std::vector<int> bCut = bestCut(newCuts, h, w);
        std::cout<<"direction: "<<bCut[0]<<", minCut: "<<bCut[1]<<", maxCut: "<<bCut[2]<<std::endl;
        mesh.cutMeshText(w,h,bCut[0],bCut[1],true);
        mesh.updateChartsFromChartsTriangles();
        mesh.cutMeshText(w,h,bCut[0],bCut[2],true);
        mesh.updateChartsFromChartsTriangles();
        std::vector<std::vector<double>> newRectangles = packRectangles(mesh, h, w, true);
        std::cout << "fin du packing" << std::endl;
        for(int i = 0; i < newRectangles.size(); i++){
            double xmin = newRectangles[i][1];
            double ymin = newRectangles[i][2];
            double xmax = newRectangles[i][3];
            double ymax = newRectangles[i][4];
            std::cout << "xmin : " << xmin << "xmax : " << xmax << "ymin : " << ymin << "ymax : " << ymax << std::endl;
        }
        moveCharts(w,h,newRectangles);
        if(uvMode){
            meshToUV();
            point3d bb(FLT_MAX,FLT_MAX,FLT_MAX) , BB(-FLT_MAX,-FLT_MAX,-FLT_MAX);
            for( unsigned int v = 0 ; v < mesh.vertices.size() ; ++v ) {
                bb = point3d::min(bb , mesh.vertices[v]);
                BB = point3d::max(BB , mesh.vertices[v]);
            }
            adjustCamera(bb,BB);
            update();
        }
        else{
            point3d bb(FLT_MAX,FLT_MAX,FLT_MAX) , BB(-FLT_MAX,-FLT_MAX,-FLT_MAX);
            for( unsigned int v = 0 ; v < mesh.vertices.size() ; ++v ) {
                bb = point3d::min(bb , mesh.vertices[v]);
                BB = point3d::max(BB , mesh.vertices[v]);
            }
            adjustCamera(bb,BB);
            update();
        }
    }

    void meshToUV(){
        copyMesh = mesh;

        mesh.triangles = mesh.triangles_text;
        for( unsigned int v = 0 ; v < mesh.textcoords.size() ; ++v ) {
            mesh.vertices[v].p = point3d( mesh.textcoords[v][0] , mesh.textcoords[v][1] , 0.0 );
        }
        mesh.chartsTriangles3D = mesh.chartsTriangles;
    }

    QSize resizeAtlas(){
        int sizeX = atlas.size().width();
        int sizeY = atlas.size().height();
        int xMin= sizeX;
        int yMin= sizeY;
        int xMax=0;
        int yMax=0;
        for(int x = 0; x<sizeX; x++){
            for(int y = 0; y<sizeY; y++){
                if(atlas.pixelColor(x,y).red() != 255 || atlas.pixelColor(x,y).green() != 255 || atlas.pixelColor(x,y).blue() != 255){
                    xMin = std::min(x,xMin);
                    yMin = std::min(y,yMin);
                    xMax = std::max(x,xMax);
                    yMax = std::max(y,yMax);
                }
            }
        }
        QImage newAtlas = QImage(xMax-xMin+1,yMax-yMin+1,QImage::Format_RGB32);
        newAtlas.fill(QColor(255,255,255));
        for(int x = 0; x<=xMax-xMin; x++){
            for(int y = 0; y<=yMax-yMin; y++){
                newAtlas.setPixelColor(x,y,atlas.pixelColor(x+xMin,y+yMin));
            }
        }
        atlas = newAtlas.copy();
        atlas.save("atlasAfterResizeBis"+QString::number(iteration)+".jpg");
        return(QSize(xMax-xMin+1, yMax-yMin+1));

    }

    void renderGeometry(int wbuffer, int hbuffer)
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
            atlas.save("atlasBoxCutterBis"+QString::number(iteration)+".jpg");

            glClearColor(back[0], back[1], back[2], back[3]);
            glPopAttrib();
            _framebuffer->release();
            delete _framebuffer;
            glViewport(0,0,this->size().width(),this->size().height());
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
                    emptyPixels.push_back(QPoint(x,y));
                    if(x<wbuffer-1){
                        skipBuffer[x+1][y].setX(skipBuffer[x][y].x()+1);
                    }
                }
                else{
                    fullPixels.push_back(QPoint(x,y));
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

            int bestLength;
            //Trouver les coupes horizontales optimales
            int bestCut0;
            if(y0 != 0 && y0 != hbuffer){
                bestCut0 = NULL;
                bestLength = hbuffer+1;
                for(int i = std::max(int(y0 - h0), 0); i < y0 + h0 + 1; i++){
                    int currentLength = computeLength(hbuffer, wbuffer, i, 1);
                    if((currentLength <= bestLength) && currentLength >= 0){
                        bestLength = currentLength;
                        bestCut0 = i;
                    }
                }
            }
            else{
                bestCut0 = y0;
            }

            int bestCut1;
            if(y1 != 0 && y1 != hbuffer){
                bestCut1 = NULL;
                bestLength = hbuffer+1;
                for(int i = y1 - h0; i < std::min(int(y1 + h0 + 1), hbuffer); i++){
                    int currentLength = computeLength(hbuffer, wbuffer, i, 1);
                    if((currentLength <= bestLength) && currentLength >= 0){
                        bestLength = currentLength;
                        bestCut1 = i;
                    }
                }
            }
            else{
                bestCut1 = y1;
            }

            //Trouver les coupes verticales optimales
            int bestCut2;
            if(x0 != 0 && x0 != wbuffer){
                bestCut2 = NULL;
                bestLength = wbuffer+1;
                for(int i = std::max(int(x0 - w0), 0); i < x0 + w0 + 1; i++){
                    int currentLength = computeLength(hbuffer, wbuffer, i, 0);
                    if((currentLength <= bestLength) && currentLength >= 0){
                        bestLength = currentLength;
                        bestCut2 = i;
                    }
                }
            }
            else{
                bestCut2 = x0;
            }

            int bestCut3;
            if(x1 != 0 && x1 != wbuffer){
                bestCut3 = NULL;
                bestLength = wbuffer+1;
                for(int i = x1 - w0; i < std::min(int(x1 + w0 + 1), wbuffer); i++){
                    int currentLength = computeLength(hbuffer, wbuffer, i, 0);
                    if((currentLength <= bestLength) && currentLength >= 0){
                        bestLength = currentLength;
                        bestCut3 = i;
                    }
                }
            }
            else{
                bestCut3 = x1;
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
        std::vector<std::vector<double>> packRectangles(Mesh currentMesh, int hbuffer, int wbuffer, bool test=false){
            std::vector<std::vector<double>> newRectangles;
            std::vector<std::vector<double>> sizes;

            currentMesh.updateBoundingBoxes();
            std::vector<std::vector<double>> boundingBoxes = currentMesh.chartsBoundingBoxes;

            //On récupère l'aire, la longueur et la largeur de chaque rectangle
            for(int i = 0; i < boundingBoxes.size(); i++){
                double x0 = boundingBoxes[i][0];
                double y0 = boundingBoxes[i][1];
                double x1 = boundingBoxes[i][2];
                double y1 = boundingBoxes[i][3];
                double width = fabs(x1 - x0);
                double height = fabs(y1 - y0);
                std::vector<double> size;
                size.push_back(i);
                size.push_back(width*height);
                size.push_back(std::max(width, height));
                size.push_back(std::min(width, height));
                sizes.push_back(size);
            }

            struct {
                    bool operator()(std::vector<double> size1, std::vector<double> size2) const  //opérateur qui compare l'aire de deux rectangles pour les trier
                    {
                        double s1 = size1[1];
                        double s2 = size2[1];

                        return s1 > s2;
                    }
                } areaIsBigger;

            //On range les rectangles par aire croissante, puis par plus grand côté croissant
            std::sort(sizes.begin(), sizes.end(), areaIsBigger);

            //On initialise un arbre avec une taille assez grande (à discuter)
            PackingTree * packingTree = new PackingTree(1.0, 1.0);

            //On calcule la nouvelle place de chacun des rectangles grâce à l'arbre
            for(int i = 0; i < sizes.size(); i++){
                //std::cout<<boundingBoxes[sizes[i][0]][0]<<" "<<boundingBoxes[sizes[i][0]][1]<<" "<<boundingBoxes[sizes[i][0]][2]<<" "<<boundingBoxes[sizes[i][0]][3]<<std::endl;
                std::vector<double> newRectangle = packingTree->place(sizes[i]);
                //std::cout<<"devient "<<newRectangle[1]<<" "<<newRectangle[2]<<" "<<newRectangle[3]<<" "<<newRectangle[4]<<std::endl;
                if(newRectangle.empty()){
                    std::cout << "La coupe n'est pas valable" << std::endl;
                    std::vector<std::vector<double>> nullVec;
                    return nullVec;
                }
                else{
                    newRectangles.push_back(newRectangle);
                }
            }
            if(test){
             std::cout<<"newRectangle.size() à la fin de packRectangles: "<<newRectangles.size()<<std::endl;
            }
            std::cout << "la coupe est valable" << std::endl;
            return newRectangles;
        }

        //Après packing, calcul de la taille nécessaire pour la nouvelle image
        std::vector<double> computeImageSize(std::vector<std::vector<double>> rectangles, int hbuffer, int wbuffer){
            double xmax = 0;
            double ymax = 0;
            std::vector<double> newSize;

            for(int i = 0; i < rectangles.size(); i++){
                std::vector<double> rectangle = rectangles[i];
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
            double bestScore = 0;
            int orientation, min, max;
            int bestBoundingBox = 0;
            double meshArea = mesh.getMeshArea();
            for(int i = 0; i < newCuts.size(); i++){
                Mesh currentMeshV = mesh;
                Mesh currentMeshH = mesh;

                std::vector<int> currentCut = newCuts[i];
                //packVerticalCut((double)currentCut[2]/(double)wbuffer, (double)currentCut[3]/(double)wbuffer, currentChartV);
                //std::cout << "Taille des charts V après: " << currentChartV.size() << std::endl;
                //packHorizontalCut((double)currentCut[0]/(double)hbuffer, (double)currentCut[1]/(double)hbuffer, currentChartH);
                //std::cout << "Taille des charts H après: " << currentChartH.size() << std::endl;

                //remplace les packHorizontalCut et packVerticalCut
                double lV = 0;
                lV += currentMeshV.cutMeshText(wbuffer,hbuffer,0,currentCut[2]);
                lV += currentMeshV.cutMeshText(wbuffer,hbuffer,0,currentCut[3]);
                currentMeshV.updateChartsFromChartsTriangles();
                std::cout << "lV " << lV << std::endl;

                double lH = 0;
                lH += currentMeshH.cutMeshText(wbuffer,hbuffer,1,currentCut[0]);
                lH += currentMeshH.cutMeshText(wbuffer,hbuffer,1,currentCut[1]);
                currentMeshH.updateChartsFromChartsTriangles();
                std::cout << "lH " << lH << std::endl;


                std::cout<<"on passe à packRectangles dans bcut"<<std::endl;

                std::vector<std::vector<double>> newRectanglesV = packRectangles(currentMeshV, hbuffer, wbuffer);
                std::vector<std::vector<double>> newRectanglesH = packRectangles(currentMeshH, hbuffer, wbuffer);

                if(not newRectanglesV.empty()){
                    std::vector<double> sizeV = computeImageSize(newRectanglesV, hbuffer, wbuffer);
                    double packEffV = meshArea*100/(sizeV[0] * sizeV[1]);
                    double scoreV = packEffV/std::pow(lV, 0);
                    std::cout << "Taille verticale : " << sizeV[0] << "x" << sizeV[1] << std::endl;
                    std::cout << "Score V " << scoreV << std::endl;
                    if(scoreV > bestScore){
                        bestBoundingBox = i;
                        std::cout << "Nouvelle meilleure coupe avec score de : " << scoreV << " et efficiency de " << packEffV << "%" << std::endl;
                        bestScore = scoreV;
                        orientation = 0;
                        min = newCuts[i][2];
                        max = newCuts[i][3];
                        std::cout<<"min cut: "<<min<<std::endl;
                        std::cout<<"max cut: "<<max<<std::endl;
                    }
                }
                if(not newRectanglesH.empty()){
                    std::vector<double> sizeH = computeImageSize(newRectanglesH, hbuffer, wbuffer);
                    double packEffH = meshArea*100/(sizeH[0] * sizeH[1]);
                    double scoreH = packEffH/std::pow(lH, 0);
                    std::cout << "Score H " << scoreH << std::endl;
                    std::cout << "Taille horizontale : " << sizeH[0] << "x" << sizeH[1] << std::endl;
                    std::cout<<"newCuts["<<i<<"][0]: "<<newCuts[i][0]<<", newCuts["<<i<<"][1]: "<<newCuts[i][1]<<", newCuts["<<i<<"][2]: "<<newCuts[i][2]<<", newCuts["<<i<<"][3]: "<<newCuts[i][3]<<std::endl;

                    if(scoreH > bestScore){
                        bestBoundingBox = i;
                        std::cout << "Nouvelle meilleure coupe avec score de : " << scoreH << " et efficiency de " << packEffH << "%" << std::endl;
                        bestScore = scoreH;
                        orientation = 1;
                        min = newCuts[i][0];
                        max = newCuts[i][1];
                        std::cout<<"min cut courant: "<<min<<std::endl;
                        std::cout<<"max cut courant: "<<max<<std::endl;
                    }
                }
            }
            std::cout<<"on choisit la Bounding Box "<<bestBoundingBox<<std::endl;
            std::cout<<"orientation de la cut: "<<orientation<<std::endl;
            std::cout<<"min cut final: "<<min<<std::endl;
            std::cout<<"max cut final: "<<max<<std::endl;
            std::cout<<"bestScore: "<<bestScore<<std::endl;
            std::vector<int> bestCut;

            bestCut.push_back(orientation);
            bestCut.push_back(min);
            bestCut.push_back(max);

            return bestCut;
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
                std::vector<Triangle> chartTriangles3D = std::vector<Triangle>();
                std::vector<double> boundingBox = std::vector<double>();
                boundingBox.push_back(mesh.textcoords[k][0]);
                boundingBox.push_back(mesh.textcoords[k][1]);
                boundingBox.push_back(mesh.textcoords[k][0]);
                boundingBox.push_back(mesh.textcoords[k][1]);
                mesh.charts.push_back(chart);
                mesh.chartsTriangles.push_back(chartTriangles);
                mesh.chartsTriangles3D.push_back(chartTriangles3D);
                mesh.chartsBoundingBoxes.push_back(boundingBox);

                examineVertex(k);
            }
        }
        std::cout<<"nombre de charts du mesh: "<<mesh.charts.size()<<std::endl;

        for(int i = 0; i<mesh.triangles_text.size(); i++){
            int chartIndex = mesh.chartOfVertex[mesh.triangles_text[i][0]];
            mesh.chartsTriangles[chartIndex].push_back(mesh.triangles_text[i]);
            mesh.chartsTriangles3D[chartIndex].push_back(mesh.triangles[i]);
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

   void moveCharts(int wbuffer, int hbuffer, std::vector<std::vector<double>> newChartsBoundingBoxes){
       for(int i = 0; i<newChartsBoundingBoxes.size(); i++){
           std::vector<int> chartToMove = mesh.charts[newChartsBoundingBoxes[i][0]];
           mesh.updateBoundingBoxes();
           std::vector<std::vector<double>> oldBoxes = mesh.chartsBoundingBoxes;
           if((newChartsBoundingBoxes[i][3]-newChartsBoundingBoxes[i][1] > 0.99*oldBoxes[newChartsBoundingBoxes[i][0]][2]-oldBoxes[newChartsBoundingBoxes[i][0]][0])&& (newChartsBoundingBoxes[i][3]-newChartsBoundingBoxes[i][1] < 1.01*oldBoxes[newChartsBoundingBoxes[i][0]][2]-oldBoxes[newChartsBoundingBoxes[i][0]][0])){
               std::cout<<"pas de rotation"<<std::endl;
               double moveX = newChartsBoundingBoxes[i][1]-oldBoxes[newChartsBoundingBoxes[i][0]][0];
               double moveY = newChartsBoundingBoxes[i][2]-oldBoxes[newChartsBoundingBoxes[i][0]][1];
               mesh.moveVertices(newChartsBoundingBoxes[i][0], moveX, moveY);
           }
           else{
               std::cout<<"rotation"<<std::endl;
               double moveX = newChartsBoundingBoxes[i][1]-oldBoxes[newChartsBoundingBoxes[i][0]][1];
               double moveY = newChartsBoundingBoxes[i][2]-oldBoxes[newChartsBoundingBoxes[i][0]][0];
               mesh.invertXY(newChartsBoundingBoxes[i][0]);
               mesh.moveVertices(newChartsBoundingBoxes[i][0], moveX, moveY);
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
        int n = mesh.chartsTriangles3D.size();
        if(n == 0){
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

        else{
            glEnable(GL_DEPTH_TEST);
            glEnable( GL_LIGHTING );
            glBegin(GL_TRIANGLES);
            double clr = 1./n;
            for(int i = 0; i < n; i++){
                //glColor3f(clr*i,1-clr*i,clr*i);
                if(i < 30){
                 glColor3f(RGB::color[i][0],RGB::color[i][1],RGB::color[i][2]);
                }
                else{
                  glColor3f(clr*i,1-clr*i,clr*i);
                }
                for( unsigned int t = 0 ; t < mesh.chartsTriangles3D[i].size() ; ++t ) {
                    /*std::cout<<t<<" ème triangle à dessiner"<<std::endl;
                    std::cout<<"mesh.chartsTriangles3D[i][t][0]: "<<mesh.chartsTriangles3D[i][t][0]<<std::endl;
                    std::cout<<"mesh.chartsTriangles3D[i][t][1]: "<<mesh.chartsTriangles3D[i][t][1]<<std::endl;
                    std::cout<<"mesh.chartsTriangles3D[i][t][2]: "<<mesh.chartsTriangles3D[i][t][2]<<std::endl;
                    std::cout<<"mesh.vertices.size(): "<<mesh.vertices.size()<<std::endl;*/
                    point3d const & p0 = mesh.vertices[ mesh.chartsTriangles3D[i][t][0] ].p;
                    point3d const & p1 = mesh.vertices[ mesh.chartsTriangles3D[i][t][1] ].p;
                    point3d const & p2 = mesh.vertices[ mesh.chartsTriangles3D[i][t][2] ].p;
                    point3d const & n = point3d::cross( p1-p0 , p2-p0 ).direction();
                    glNormal3f(n[0],n[1],n[2]);
                    glVertex3f(p0[0],p0[1],p0[2]);
                    glVertex3f(p1[0],p1[1],p1[2]);
                    glVertex3f(p2[0],p2[1],p2[2]);
                }
            }
            glEnd();
        }
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
        else if(event->key() == Qt::Key_M){
            if(uvMode){
                mesh = copyMesh;
                std::cout<<"mesh.vertices.size(): "<<mesh.vertices.size()<<std::endl;
                point3d bb(FLT_MAX,FLT_MAX,FLT_MAX) , BB(-FLT_MAX,-FLT_MAX,-FLT_MAX);
                for( unsigned int v = 0 ; v < mesh.vertices.size() ; ++v ) {
                    bb = point3d::min(bb , mesh.vertices[v]);
                    BB = point3d::max(BB , mesh.vertices[v]);
                }
                adjustCamera(bb,BB);
                update();
                uvMode = false;

            }
            else{
                uvMode = true;
                meshToUV();
                point3d bb(FLT_MAX,FLT_MAX,FLT_MAX) , BB(-FLT_MAX,-FLT_MAX,-FLT_MAX);
                for( unsigned int v = 0 ; v < mesh.vertices.size() ; ++v ) {
                    bb = point3d::min(bb , mesh.vertices[v]);
                    BB = point3d::max(BB , mesh.vertices[v]);
                }
                adjustCamera(bb,BB);
                update();
            }
            /*update();
            uvMode = !uvMode;*/
        }

        else if(event->key() == Qt::Key_B){
            boxCutter();
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

                computeMeshTextCharts();

                std::cout<<"mesh.vertices.size(): "<<mesh.vertices.size()<<std::endl;
                std::cout<<"mesh.textcoords.size(): "<<mesh.textcoords.size()<<std::endl;
                std::cout<<"mesh.triangles.size(): "<<mesh.triangles.size()<<std::endl;
                std::cout<<"mesh.triangles_text.size(): "<<mesh.triangles_text.size()<<std::endl;
                }

                point3d bb(FLT_MAX,FLT_MAX,FLT_MAX) , BB(-FLT_MAX,-FLT_MAX,-FLT_MAX);
                for( unsigned int v = 0 ; v < mesh.vertices.size() ; ++v ) {
                    bb = point3d::min(bb , mesh.vertices[v]);
                    BB = point3d::max(BB , mesh.vertices[v]);
                }
                adjustCamera(bb,BB);
                update();
            }
            else {
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
