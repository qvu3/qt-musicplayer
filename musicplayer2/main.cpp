#include <QApplication>
#include "mainwindow.h"  // this must match your header file name exactly

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MusicPlayer window;  // Make sure the class is called MusicPlayer in mainwindow.h
    window.show();

    return app.exec();
}
