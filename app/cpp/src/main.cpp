#include "mainwindow.hpp"
#include "codeforceswrapper.hpp"

#include <QApplication>

Q_DECLARE_METATYPE(CodeforcesProblem);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("CorgIDE");
    a.setOrganizationName("dj1vs");
    
    MainWindow w;
    w.show();
    return a.exec();
}
