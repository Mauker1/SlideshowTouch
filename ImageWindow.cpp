#include <QtGui>
#include "imageWindow.h"
#include <iostream>

ImageWindow::ImageWindow(){
    // Widget usado para pegar a resolucao da tela.
    QDesktopWidget desk;

    // Definindo as variaveis do header
    mainWindow = new QLabel;
    mainWindow->setBackgroundRole(QPalette::Base); // Setar background.
    mainWindow->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);
    mainWindow->setScaledContents(true); // Auto escalar as imagens.
    mainWindow->grabGesture(Qt::PinchGesture); // Aceitar gesto swipe.
    mainWindow->grabGesture(Qt::SwipeGesture); // Aceitar gesto swipe.
    mainWindow->setAttribute(Qt::WA_AcceptTouchEvents); // Aceitar eventos de toque.


    imageIndex = 0;
    imageCount = 0;

    createActions();
    createMenu();

    setCentralWidget(mainWindow);
    setWindowTitle(tr("SlideShow"));


    resize(desk.width()/2,desk.height()/2);
}

void ImageWindow::setDir(){
    curDir = QFileDialog::getExistingDirectory(this,tr("Selecionar diretorio"),"",QFileDialog::ShowDirsOnly);

    mainWindow->installEventFilter(this);

    if (curDir.isNull()){
        QMessageBox::information(this,tr("Slideshow"),tr("Nao foi selecionado um diretorio!"));
        imageCount = 0;
        imageIndex = 0;
        imagesList.clear();
        mainWindow->clear();
        mainWindow->removeEventFilter(this);
        return;
    }
    else{
        QDir myDir(curDir);
        QStringList filtro;
        filtro << "*.JPG" << "*.PNG";
        myDir.setNameFilters(filtro);
        imagesList = myDir.entryList();

        if (imagesList.isEmpty()){
            QMessageBox::information(this,tr("Slideshow"),tr("Nao existem imagens no diretório selecionado!"));
            imageCount = imagesList.size();
            imageIndex = 0;
            mainWindow->clear();
            mainWindow->removeEventFilter(this);
            return;
        }
        else{
            imageCount = imagesList.size();
            imageIndex = 0;
            openImage();
        }
    }
}

void ImageWindow::openImage(){
    QString fileName = curDir;

    fileName.append("/");
    fileName.append(imagesList.at(imageIndex).toUtf8().constData());

    if (!fileName.isEmpty()){
        QImage image(fileName);

        if (image.isNull()){
            QMessageBox::information(this,tr("Slideshow"),tr("Nao foi possível abrir a foto %1.").arg(fileName));
            return;
        }
        else {
            //mainWindow->clear();
            mainWindow->setPixmap(QPixmap::fromImage(image));
        }
    }
    else{
        QMessageBox::information(this,tr("Slideshow"),tr("Erro ao abrir foto %1.").arg(fileName));
        return;
    }
}

void ImageWindow::nextImage(){
    if (imageIndex + 1 < imageCount){
        imageIndex++;
        openImage();
    }
    else{
        QMessageBox::information(this,tr("Slideshow"),tr("Fim das imagens!"));
        return;
    }
}

void ImageWindow::previousImage(){
    if (imageIndex - 1 >= 0){
        imageIndex--;
        openImage();
    }
    else{
        QMessageBox::information(this,tr("Slideshow"),tr("Essa é a primeira imagem!"));
        return;
    }
}

void ImageWindow::checkGestureEvent(QGestureEvent * event){
    std::cout << "Gesture event was detected successfully!" << std::endl;
    if (QGesture *swipe = event->gesture(Qt::SwipeGesture)){
        swipeEvent(static_cast<QSwipeGesture *>(swipe));
    }
    else if (QGesture *pinch = event->gesture(Qt::PinchGesture)){
        pinchEvent(static_cast<QPinchGesture *>(pinch));
    }
}

void ImageWindow::swipeEvent(QSwipeGesture *swipeGesture){
    if (swipeGesture->state() == Qt::GestureFinished){
        if (swipeGesture->horizontalDirection() == QSwipeGesture::Left || swipeGesture->verticalDirection() == QSwipeGesture::Up){
            previousImage();
        }
        else {
            nextImage();
        }
    }

}

void ImageWindow::pinchEvent(QPinchGesture * pinch){
    if (pinch->state() == Qt::GestureFinished){
        std::cout << "Pinch finalizado! " << "Rotation angle: " << pinch->rotationAngle() << std::endl;

        if (pinch->rotationAngle() > 0){
            nextImage();
        }
        else if (pinch->rotationAngle() < 0){
            previousImage();
        }
    }
}

void ImageWindow::checkMouseClick(int pos){
    std::cout << "Mouse event! Mouse Clicked!" << std::endl;
    if (pos <= this->width()/2){
        previousImage();
    }
    else {
        nextImage();
    }
}

bool ImageWindow::eventFilter(QObject *watched, QEvent *event){
    if (qobject_cast<QLabel*>(watched) != NULL && event->type() == QEvent::MouseButtonPress){

        const QMouseEvent *me = static_cast<const QMouseEvent*>(event);

        const QPoint point = me->pos();
        int pos = point.x();

        checkMouseClick(pos);
        return true;
    }
    else if (qobject_cast<QLabel*>(watched) != NULL && event->type() == QEvent::NativeGesture){

        std::cout << "Native gesture detected! Type: " << event->type() << std::endl;
        QGestureEvent *ge = static_cast<const QGestureEvent*>(event);
        checkGestureEvent(ge);
        return true;
    }
    else if (qobject_cast<QLabel*>(watched) != NULL && event->type() == QEvent::Gesture){
        std::cout << "Gesture detected! Type: " << event->type() << std::endl;
        QGestureEvent *ge = static_cast<const QGestureEvent*>(event);
        checkGestureEvent(ge);
        return true;
    }
    else{
        return false;
    }
}

void ImageWindow::quit(){
    if (QMessageBox::question(this,tr("Sair?"),tr("Voce realmente gostaria de fechar?"),QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes){
            qApp->quit();
        }
}

void ImageWindow::createMenu(){
    fileMenu = menuBar()->addMenu(tr("&Arquivo"));
    controlMenu = menuBar()->addMenu(tr("&Controle"));

    fileMenu->addAction(getCurDirAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    controlMenu->addAction(nextImageAction);
    controlMenu->addAction(previousImageAction);
}

void ImageWindow::createActions(){
    getCurDirAction = new QAction (tr("Trocar diretorio base"),this);
    getCurDirAction->setShortcut(tr("Ctrl+O"));

    nextImageAction = new QAction(tr("&Proxima imagem"),this);
    nextImageAction->setShortcut(tr("Ctrl+M"));

    previousImageAction = new QAction(tr("I&magem anterior"),this);
    previousImageAction->setShortcut(tr("Ctrl+N"));

    exitAction = new QAction(tr("F&echar"),this);
    exitAction->setShortcut(tr("Ctrl+W"));

    connect(getCurDirAction,SIGNAL(triggered()),this,SLOT(setDir()));
    connect(exitAction,SIGNAL(triggered()),this,SLOT(quit()));

    connect(nextImageAction,SIGNAL(triggered()),this,SLOT(nextImage()));
    connect(previousImageAction,SIGNAL(triggered()),this,SLOT(previousImage()));

    //connect(mainWindow,SIGNAL(clicked(const QPoint &)),this,SLOT(checkMouseClick()));

}
