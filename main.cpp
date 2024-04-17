#include "onlinemp3widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OnlineMp3Widget w;
    w.show();
    return a.exec();
}
