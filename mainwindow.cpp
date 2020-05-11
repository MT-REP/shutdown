#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qt_windows.h>
#include <QTimer>
#include <QDebug>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mMotusSocket.initSocket(10010);
    connect(&mMotusSocket,SIGNAL(sendRecvData(char*,int)),this,SLOT(recvData(char*,int)));
    setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    createMenu();
    this->showMinimized();

}

//创建菜单
void MainWindow::createMenu()
{
    mMenu = new QMenu(this);
    //退出动作
    mExitAppAction = new QAction("退出",this);
    connect(mExitAppAction,SIGNAL(triggered()),this,SLOT(on_exitAppAction()));
    //新增菜单项---退出程序
    mMenu->addAction(mExitAppAction);
    //把QMenu赋给QSystemTrayIcon对象
    mQSystemTrayIcon.setContextMenu(mMenu);

    //新建托盘要显示的icon
    QIcon icon = QIcon(":/shutdown.png");
    //将icon设到QSystemTrayIcon对象中
    mQSystemTrayIcon.setIcon(icon);
    //当鼠标移动到托盘上的图标时，会显示此处设置的内容
    mQSystemTrayIcon.setToolTip("联机关机程序");
    connect(&mQSystemTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,SLOT(on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));
    //在系统托盘显示此对象
    mQSystemTrayIcon.show();
}

//析构函数
MainWindow::~MainWindow()
{
    delete ui;
}

//最小化界面到托盘
void MainWindow::minWindow()
{
    //隐藏程序主窗口
    this->hide();
}

//托盘相应槽函数
void MainWindow::on_activatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch(reason){
    case QSystemTrayIcon::Trigger:
        //单击托盘图标
        break;
    case QSystemTrayIcon::DoubleClick:
        //双击托盘图标
        //双击后显示主程序窗口
        ::SetWindowPos(HWND(this->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        // 取消窗口置顶
        ::SetWindowPos(HWND(this->winId()), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        this->showNormal();
        break;
    default:
        break;
    }
}

//接收数据信息
void MainWindow::recvData(char *data,int lengh)
{
    static char rdata[200]={0};
    QString str;
    memset(rdata,0,200);
    memcpy(rdata,data,lengh);
    str=rdata;
    if(str.compare("shutdown")==0)
    {
        closeComputer();
    }
}

//关闭电脑
void MainWindow::closeComputer()
{
    this->close();
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    //获取进程标志
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    return ;
    //获取关机特权的LUID
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    //获取这个进程的关机特权
    AdjustTokenPrivileges(hToken, false, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
    if (GetLastError() != ERROR_SUCCESS) return ;
    // 强制关闭计算机
    if( !ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0))
        return ;
}

//窗口状态改变
void MainWindow::changeEvent(QEvent* event)
{
    if(event->type()!=QEvent::WindowStateChange) return;
    //Qt::WindowMaximized 为最大化时触发，Qt::WindowMinimized 是最小化时触发
    if(this->windowState()==Qt::WindowMinimized)
    {
        minWindow();
        QTimer::singleShot(10, this, SLOT(hide()));
        return;
    }
}

//关闭按键
void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}

//程序退出
void MainWindow::on_exitAppAction()
{
    mQSystemTrayIcon.hide();
    mQSystemTrayIcon.deleteLater();
    exit(0);
}
