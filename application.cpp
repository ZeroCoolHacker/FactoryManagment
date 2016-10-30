#include "application.h"

Application::Application(int& argc, char** argv) :
    QApplication(argc, argv)
{
    m_dontShowMainWindow = true;
    // Set organisation and application names
    setApplicationName("786Plastics");
    setApplicationVersion("2.01");
    setOrganizationName("Shaprianz");
    setApplicationDisplayName("786Plastics");
    setWindowIcon(QIcon(":/images/img/main.png"));

    // Show login dialog
    login = new LoginDialog();
    login->show();
    login->setAttribute(Qt::WA_DeleteOnClose);
    if(login != Q_NULLPTR)
        m_dontShowMainWindow = false;

    connect(this, SIGNAL(lastWindowClosed()), this, SLOT(quit()));
}

Application::~Application()
{
    delete login;
}
