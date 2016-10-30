#include "logindialog.h"
#include "application.h"
#include <QFile>
#include <QMessageBox>

int main(int argc, char *argv[])
{

    Application a(argc, argv);

    if(a.dontShowMainWindow())
        return 0;

    return a.exec();
}
