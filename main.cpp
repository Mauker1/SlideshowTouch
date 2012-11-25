#include <QApplication>
#include "imageWindow.h"


int main (int argc, char* argv[]){
    QApplication app(argc,argv);

    ImageWindow imagesWindow;

    imagesWindow.show();
    imagesWindow.installEventFilter(&imagesWindow);
    imagesWindow.setDir();


    return app.exec();
}
