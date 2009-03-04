#include <QApplication>

#include "main_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("Debilski");
    app.setOrganizationDomain("debilski.de");
    app.setApplicationName("Wellenprogramm");
    QSettings settings;



    MainWindow *dialog = new MainWindow();

    dialog->raise();
    dialog->show();
    dialog->activateWindow();


    app.connect( &app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()) );

    return app.exec();
}
