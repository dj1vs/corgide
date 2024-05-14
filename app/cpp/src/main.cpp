#include "mainwindow.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("CorgIDE");
    a.setOrganizationName("dj1vs");
    
    MainWindow w;
    w.show();
    
    return a.exec();
}
