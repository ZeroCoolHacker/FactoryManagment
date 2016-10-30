#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include "logindialog.h"
#include <QApplication>


class Application : public QApplication
{
    Q_OBJECT

public:
    explicit Application(int& argc, char** argv);
    virtual ~Application();

    bool dontShowMainWindow() { return m_dontShowMainWindow; }

private:
    bool m_dontShowMainWindow;
    LoginDialog *login;
};


#endif // APPLICATION_H
