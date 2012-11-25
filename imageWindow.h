#ifndef IMAGEWINDOW_H
#define IMAGEWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QDir>
#include <QGesture>
#include <QEvent>

class ImageWindow : public QMainWindow
{
    Q_OBJECT

public:
    ImageWindow();

public slots:
    void setDir();

private slots:

    void openImage();
    void nextImage();
    void previousImage();
    void checkMouseClick(int);
    void quit();

protected:
    bool eventFilter(QObject *, QEvent *);

private:
    void checkGestureEvent(QGestureEvent *);
    void swipeEvent(QSwipeGesture *);
    void pinchEvent(QPinchGesture *);

    void createMenu();
    void createActions();

    QLabel *mainWindow;
    QString curDir;
    QStringList imagesList;
    int imageIndex;
    int imageCount;

    QAction *getCurDirAction;
    QAction *nextImageAction;
    QAction *previousImageAction;
    QAction *exitAction;

    QMenu *fileMenu;
    QMenu *controlMenu;

};

#endif // IMAGEWINDOW_H
