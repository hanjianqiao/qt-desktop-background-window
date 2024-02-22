#include <QWindow>
#include <QScreen>
#include <QGuiApplication>
#include <qt_windows.h>
#include "desktopbackgroundwindow.h"

namespace h {
DesktopBackgroundWindowManager *DesktopBackgroundWindowManager::s_instance = nullptr;

void DesktopBackgroundWindowManager::initialize(){
    Q_ASSERT(s_instance == nullptr);
    s_instance = new DesktopBackgroundWindowManager(nullptr);
}

DesktopBackgroundWindowManager *DesktopBackgroundWindowManager::singleton(){
    return s_instance;
}

QList<QScreen*> DesktopBackgroundWindowManager::screens() const{
    return QGuiApplication::screens();
}

void DesktopBackgroundWindowManager::setScreenBackgroundWindow(QScreen *toScreen, DesktopBackgroundWindow *backgroundWindow){

}

DesktopBackgroundWindowManager::DesktopBackgroundWindowManager(QObject *parent)
    : QObject{parent}
{
    QObject::connect(qApp, &QGuiApplication::screenAdded, this, &DesktopBackgroundWindowManager::onScreenAdded);
    QObject::connect(qApp, &QGuiApplication::screenRemoved, this, &DesktopBackgroundWindowManager::onScreenRemoved);
    QObject::connect(qApp, &QGuiApplication::primaryScreenChanged, this, &DesktopBackgroundWindowManager::onPrimaryScreenChanged);
}

DesktopBackgroundWindowManager::~DesktopBackgroundWindowManager(){}

void DesktopBackgroundWindowManager::onScreenAdded(QScreen *screen){

}

void DesktopBackgroundWindowManager::onScreenRemoved(QScreen *screen){

}

void DesktopBackgroundWindowManager::onPrimaryScreenChanged(QScreen *screen){

}

DesktopBackgroundWindow::DesktopBackgroundWindow(QScreen *screen)
    : QWidget{nullptr}, m_screen{screen}
{
    QObject::connect(screen, &QScreen::geometryChanged, this, &DesktopBackgroundWindow::onGeometryChanged);
    QObject::connect(screen, &QScreen::availableGeometryChanged, this, &DesktopBackgroundWindow::onAvailableGeometryChanged);
    QObject::connect(screen, &QScreen::physicalSizeChanged, this, &DesktopBackgroundWindow::onPhysicalSizeChanged);
    QObject::connect(screen, &QScreen::physicalDotsPerInchChanged, this, &DesktopBackgroundWindow::onPhysicalDotsPerInchChanged);
    QObject::connect(screen, &QScreen::logicalDotsPerInchChanged, this, &DesktopBackgroundWindow::onLogicalDotsPerInchChanged);
    QObject::connect(screen, &QScreen::virtualGeometryChanged, this, &DesktopBackgroundWindow::onVirtualGeometryChanged);
    QObject::connect(screen, &QScreen::primaryOrientationChanged, this, &DesktopBackgroundWindow::onPrimaryOrientationChanged);
    QObject::connect(screen, &QScreen::orientationChanged, this, &DesktopBackgroundWindow::onOrientationChanged);
    QObject::connect(screen, &QScreen::refreshRateChanged, this, &DesktopBackgroundWindow::onRefreshRateChanged);
}

void DesktopBackgroundWindow::becomeDesktopBackgroundLayer(){
    setWindowFlags(Qt::FramelessWindowHint);
    setStyleSheet("{background: red;}");

    WId wid = winId();
    // qDebug() << "必须获取winID()才能使widget变成native window，官方文档里说的几种方法不太好用" << wid;
    // Message to `Progman` to spawn a `WorkerW`
    int WM_SPAWN_WORKER = 0x052C;
    // find `Progman`
    HWND hProgman= FindWindow(L"Progman", nullptr);
    // instruct program to create a `WorkerW` between wallpaper and icons
    SendMessageTimeout(
        hProgman,
        WM_SPAWN_WORKER,
        NULL,
        NULL,
        SMTO_NORMAL,
        1000,
        NULL);
    // find the newly created `WorkerW`
    static HWND hWorkerW = nullptr;
    EnumWindows([](HWND topHandle, LPARAM topParamHandle) {
        HWND shellDllDefView = FindWindowEx(topHandle, nullptr, L"SHELLDLL_DefView", nullptr);
        if (shellDllDefView != nullptr) {
            hWorkerW = FindWindowEx(nullptr, topHandle, L"WorkerW", nullptr);
        }
        return TRUE;
    }, NULL);
    // windows在多显示器的情况行，这将是一个直接包括所有显示器大小的窗口
    HWND deskHWnd = hWorkerW;
    QWindow* deskTopNativeWindow = QWindow::fromWinId(reinterpret_cast<WId>(deskHWnd));//以指定HWND创建窗口的静态函数
    QWindow *whnd = windowHandle();
    whnd->setParent(deskTopNativeWindow);
}

void DesktopBackgroundWindow::onGeometryChanged(const QRect &geometry){
    qDebug() << "geometryChanged";
}
void DesktopBackgroundWindow::onAvailableGeometryChanged(const QRect &geometry){
    qDebug() << "availableGeometryChanged";
}
void DesktopBackgroundWindow::onPhysicalSizeChanged(const QSizeF &size){
    qDebug() << "physicalSizeChanged";
}
void DesktopBackgroundWindow::onPhysicalDotsPerInchChanged(qreal dpi){
    qDebug() << "physicalDotsPerInchChanged";
}
void DesktopBackgroundWindow::onLogicalDotsPerInchChanged(qreal dpi){
    qDebug() << "logicalDotsPerInchChanged";
}
void DesktopBackgroundWindow::onVirtualGeometryChanged(const QRect &rect){
    qDebug() << "virtualGeometryChanged";
}
void DesktopBackgroundWindow::onPrimaryOrientationChanged(Qt::ScreenOrientation orientation){
    qDebug() << "primaryOrientationChanged";
}
void DesktopBackgroundWindow::onOrientationChanged(Qt::ScreenOrientation orientation){
    qDebug() << "orientationChanged";
}
void DesktopBackgroundWindow::onRefreshRateChanged(qreal refreshRate){
    qDebug() << "refreshRateChanged";
}

}
