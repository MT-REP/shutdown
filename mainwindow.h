#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "motussocket.h"
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QMenu>
#include <QAction>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void recvData(char *data,int lengh);
    void changeEvent(QEvent* event);
    void closeEvent(QCloseEvent *event);
private slots:
    void on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);
    void on_exitAppAction();
private:
    Ui::MainWindow *ui;
    MotusSocket mMotusSocket;
    QSystemTrayIcon mQSystemTrayIcon;
    QMenu *mMenu;
    QAction *mExitAppAction;
    void closeComputer();
    void minWindow();
    void createMenu();
};

#endif // MAINWINDOW_H
