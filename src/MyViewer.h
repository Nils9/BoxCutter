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

typedef


class MyViewer : public QGLViewer , public QOpenGLFunctions_3_0
{
    Q_OBJECT

    Mesh mesh;

    QWidget * controls;

public :

    //Calcule la longueur d'une coupe verticale (o=0) ou horizontale (o=1)
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

        //Trouver les coupes verticales optimales (y fixé)
        int bestCut0 = NULL;
        int bestLength = hbuffer+1;
        std::cout << "1ere coupe verticale : " << std::endl;
        std::cout << y0 << std::endl;
        for(int i = std::max(int(y0 - h0), 0); i < y0 + h0 + 1; i++){
            int currentLength = computeLength(hbuffer, wbuffer, i, 1);
            if((currentLength < bestLength) && currentLength > 0){
                bestLength = currentLength;
                bestCut0 = i;
            }
        }
        std::cout << "Je coupe à :" << std::endl;
        std::cout << bestCut0 << std::endl;

        std::cout << "2eme coupe verticale : " << std::endl;
        std::cout << y1 << std::endl;
        int bestCut1 = NULL;
        bestLength = hbuffer+1;
        for(int i = y1 - h0; i < std::min(int(y1 + h0 + 1), hbuffer); i++){
            int currentLength = computeLength(hbuffer, wbuffer, i, 1);
            if((currentLength < bestLength) && currentLength > 0){
                bestLength = currentLength;
                bestCut1 = i;
            }
        }
        std::cout << "Je coupe à :" << std::endl;
        std::cout << bestCut1 << std::endl;

        //Trouver les coupes verticales optimales (x fixé)
        std::cout << "1ere coupe verticale : " << std::endl;
        std::cout << x0 << std::endl;
        int bestCut2 = NULL;
        bestLength = wbuffer+1;
        for(int i = std::max(int(x0 - w0), 0); i < x0 + w0 + 1; i++){
            int currentLength = computeLength(hbuffer, wbuffer, i, 0);
            if((currentLength < bestLength) && currentLength > 0){
                bestLength = currentLength;
                bestCut2 = i;
            }
        }
        std::cout << "Je coupe à :" << std::endl;
        std::cout << bestCut2 << std::endl;

        std::cout << "2eme coupe verticale : " << std::endl;
        std::cout << x1 << std::endl;
        int bestCut3 = NULL;
        bestLength = wbuffer+1;
        for(int i = x1 - w0; i < std::min(int(x1 + w0 + 1), wbuffer); i++){
            int currentLength = computeLength(hbuffer, wbuffer, i, 0);
            if((currentLength < bestLength) && currentLength > 0){
                bestLength = currentLength;
                bestCut3 = i;
            }
        }
        std::cout << "Je coupe à :" << std::endl;
        std::cout << bestCut3 << std::endl;

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

    std::vector<int> pack(int c1, int c2, int o){
        //à implémenter
    }

    std::vector<int> computeBestCut(std::vector<std::vector<int>> optimizedCuts){
        //à implémenter
    }

//    int bestEfficiency(std::vector<std::vector<int>> optimizedCuts){
//        std::vector<int> efficiency;
//        int max = 0;
//        int ind = 0;
//        for(int i = 0; i < optimizedCuts.size(); i++){
//            std::vector<int> currentCut = optimizedCuts[i];
//            int scorev = pack(currentCut[0], currentCut[1], 1);
//            int scoreh = pack(currentCut[2], currentCut[3], 0);
//            if(effv > max){
//                max = scorev;
//                ind = 2*i;
//            }
//            if(effh > max){
//                max = scoreh;
//                ind = 2*i + 1;
//            }
//        }

//        return ind;
//    }

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
                {
                    mesh.triangles = mesh.triangles_text;
                    for( unsigned int v = 0 ; v < mesh.textcoords.size() ; ++v ) {
                        mesh.vertices[v].p = point3d( mesh.textcoords[v][0] , mesh.textcoords[v][1] , 0.0 );
                    }
                }
                renderGeometry(500, 500, 1000);
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
            locateVoidBoxes(_framebuffer,wbuffer,hbuffer,threshold);


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

    void locateVoidBoxes(QGLFramebufferObject * _framebuffer, int wbuffer, int hbuffer, int threshold){
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
    std::cout<<"Before filterOverlaps: "<<emptyBoxes.size()<<std::endl;
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
    std::cout<<"After filterOverlaps: "<<emptyBoxes.size()<<std::endl;
    QImage image = _framebuffer->toImage();
    for(int i = 0; i<emptyPixels.size();i++){
        image.setPixelColor(emptyPixels[i],QColor(255,0,0));
    }
    for(int i = 0; i<emptyBoxes.size();i++){
        QPoint upLeft = emptyBoxes[i][0];
        QPoint lowRight = emptyBoxes[i][1];
        //std::cout<<"x: "<<upLeft.x()<<" y:"<<upLeft.y()<<" X:"<<lowRight.x()<<" Y:"<<lowRight.y()<<std::endl;
        for(int x = upLeft.x(); x<=lowRight.x(); x++){
            for(int y = upLeft.y(); y<=lowRight.y(); y++){
                image.setPixelColor(x,y,QColor(0,0,0));
            }
        }
    }

    //Tests Nils
    for(int i = 0; i < emptyBoxes.size(); i++){
        std::vector<QPoint> vbox = emptyBoxes[i];
        std::vector<int> bestCuts = optimizeCut(hbuffer, wbuffer, vbox);
        for(int i = 0; i < wbuffer; i++){
            image.setPixelColor(i, bestCuts[0], QColor(0,0,255));
        }
        for(int i = 0; i < wbuffer; i++){
            image.setPixelColor(i, bestCuts[1], QColor(0,0,255));
        }
        for(int i = 0; i < hbuffer; i++){
            image.setPixelColor(bestCuts[2], i, QColor(0,0,255));
        }
        for(int i = 0; i < hbuffer; i++){
            image.setPixelColor(bestCuts[3], i, QColor(0,0,255));
        }
    }


    image.save("test.jpg");
    }

    int emptyBoxSize(std::vector<QPoint> emptyBox){
        int x = emptyBox[0].x();
        int y = emptyBox[0].y();
        int X = emptyBox[1].x();
        int Y = emptyBox[1].y();
        return (X-x)*(Y-y);
    }

    float overlapAmount(std::vector<QPoint> emptyBox, std::vector<QPoint> emptyBoxPrim){
        int x = emptyBox[0].x();
        int y = emptyBox[0].y();
        int X = emptyBox[1].x();
        int Y = emptyBox[1].y();

        int xPrim = emptyBoxPrim[0].x();
        int yPrim = emptyBoxPrim[0].y();
        int XPrim = emptyBoxPrim[1].x();
        int YPrim = emptyBoxPrim[1].y();

        //std::cout<<"x: "<<x<<" y: "<<y<<" X: "<<X<<" Y: "<<Y<<std::endl;
        //std::cout<<"xPrim: "<<xPrim<<" yPrim: "<<yPrim<<" XPrim: "<<XPrim<<" YPrim: "<<YPrim<<std::endl;

        if(((X>=xPrim && X<=XPrim)||(XPrim>=x && XPrim<=X))&&((Y>=yPrim && Y<=YPrim)||(YPrim>=y && YPrim<=Y))){
            int overlapArea = (std::min(X,XPrim)-std::max(x,xPrim)+1)*(std::min(Y,YPrim)-std::max(y,yPrim)+1);
            int minArea = std::min((X-x+1)*(Y-y+1),(XPrim-xPrim+1)*(YPrim-yPrim+1));
            return (float) overlapArea/ (float) minArea;

        }
        else{
            return 0;
        }
    }


};




#endif // MYVIEWER_H
