#include <QWindow>
#include <QScreen>
#include <QGuiApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <QSettings>
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

void DesktopBackgroundWindowManager::setScreenBackgroundWidget(QScreen *toScreen, QWidget *w){
    DesktopBackgroundWindow * bgw = m_backgroundWindows[toScreen];
    bgw->layout()->addWidget(w);

    bgw->setVisible(!!w);
}

DesktopBackgroundWindowManager::DesktopBackgroundWindowManager(QObject *parent)
    : QObject{parent}
{
    // 给windows发消息，再壁纸层跟桌面图标层中间新建一个图层
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

    // 初始化每个屏幕
    for(const auto &screen : QGuiApplication::screens()){
        DesktopBackgroundWindow *w = new DesktopBackgroundWindow(screen);
        m_backgroundWindows.insert(screen, w);
        w->becomeDesktopBackgroundLayer();
    }

    QObject::connect(qApp, &QGuiApplication::screenAdded, this, &DesktopBackgroundWindowManager::onScreenAdded);
    QObject::connect(qApp, &QGuiApplication::screenRemoved, this, &DesktopBackgroundWindowManager::onScreenRemoved);
    QObject::connect(qApp, &QGuiApplication::primaryScreenChanged, this, &DesktopBackgroundWindowManager::onPrimaryScreenChanged);

    QObject::connect(qApp, &QGuiApplication::aboutToQuit, this, [this](){
        // 目前没有找到一个更好的办法，窗口关闭后，壁纸层不会还原为之前的壁纸。
        QSettings setting("HKEY_CURRENT_USER\\Control Panel\\Desktop", QSettings::NativeFormat);
        QString src = setting.value("WallPaper").toString();
        TCHAR *ptch = (TCHAR *)src.toStdWString().c_str();
        SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, ptch, 0);
        SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, ptch, 0 );
    });
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
    QObject::connect(m_screen, &QScreen::geometryChanged, this, &DesktopBackgroundWindow::onGeometryChanged);
    QObject::connect(m_screen, &QScreen::availableGeometryChanged, this, &DesktopBackgroundWindow::onAvailableGeometryChanged);
    QObject::connect(m_screen, &QScreen::physicalSizeChanged, this, &DesktopBackgroundWindow::onPhysicalSizeChanged);
    QObject::connect(m_screen, &QScreen::physicalDotsPerInchChanged, this, &DesktopBackgroundWindow::onPhysicalDotsPerInchChanged);
    QObject::connect(m_screen, &QScreen::logicalDotsPerInchChanged, this, &DesktopBackgroundWindow::onLogicalDotsPerInchChanged);
    QObject::connect(m_screen, &QScreen::virtualGeometryChanged, this, &DesktopBackgroundWindow::onVirtualGeometryChanged);
    QObject::connect(m_screen, &QScreen::primaryOrientationChanged, this, &DesktopBackgroundWindow::onPrimaryOrientationChanged);
    QObject::connect(m_screen, &QScreen::orientationChanged, this, &DesktopBackgroundWindow::onOrientationChanged);
    QObject::connect(m_screen, &QScreen::refreshRateChanged, this, &DesktopBackgroundWindow::onRefreshRateChanged);

    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setAlignment(Qt::AlignCenter);
}

void DesktopBackgroundWindow::becomeDesktopBackgroundLayer(){
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    WId wid = winId();
    // qDebug() << "必须获取winID()才能使widget变成native window，官方文档里说的几种方法不太好用" << wid;

    // find the newly created `WorkerW`
    static HWND targetHWnd = nullptr;
    EnumWindows([](HWND topHandle, LPARAM topParamHandle) {
        HWND shellDllDefView = FindWindowEx(topHandle, nullptr, L"SHELLDLL_DefView", nullptr);
        if (shellDllDefView != nullptr) {
            targetHWnd = FindWindowEx(nullptr, topHandle, L"WorkerW", nullptr);
        }
        return TRUE;
    }, NULL);


    QWindow* targetWindow = QWindow::fromWinId(reinterpret_cast<WId>(targetHWnd));//以指定HWND创建窗口的静态函数

    QWindow *whnd = windowHandle();
    whnd->setParent(targetWindow);
    whnd->setPosition(targetWindow->mapFromGlobal(m_screen->geometry().topLeft()));
    whnd->resize(m_screen->size()/QGuiApplication::primaryScreen()->devicePixelRatio()*m_screen->devicePixelRatio());
    setFixedSize(whnd->size());
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
