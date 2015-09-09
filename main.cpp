#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication::setApplicationVersion("0.0.1");
    QApplication::setOrganizationName("tianxyz");
    QApplication::setOrganizationDomain("linkopq.com");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
