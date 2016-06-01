#include <display_window.h>
#include <QtGui>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    display_window disp;
    disp.show();
    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
    int result = app.exec();
    return result;
}
