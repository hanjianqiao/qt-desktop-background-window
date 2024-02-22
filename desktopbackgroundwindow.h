#ifndef DESKTOPBACKGROUNDWINDOW_H
#define DESKTOPBACKGROUNDWINDOW_H

#include <QWidget>

namespace h {
class DesktopBackgroundWindow;

class DesktopBackgroundWindowManager : public QObject
{
    Q_OBJECT
public:
    static void initialize();
    static DesktopBackgroundWindowManager *singleton();

    QList<QScreen*> screens() const;

    void setScreenBackgroundWindow(QScreen *toScreen, DesktopBackgroundWindow *backgroundWindow);

private:
    DesktopBackgroundWindowManager(QObject *parent);
    ~DesktopBackgroundWindowManager();

private slots:
    void onScreenAdded(QScreen *screen);
    void onScreenRemoved(QScreen *screen);
    void onPrimaryScreenChanged(QScreen *screen);

private:
    static DesktopBackgroundWindowManager *s_instance;

    QHash<QScreen*, DesktopBackgroundWindow*> m_backgroundWindows;
};

class DesktopBackgroundWindow : public QWidget
{
    Q_OBJECT
public:

private:
    DesktopBackgroundWindow(QScreen *screen);
    void becomeDesktopBackgroundLayer();

signals:

private slots:
    void onGeometryChanged(const QRect &geometry);
    void onAvailableGeometryChanged(const QRect &geometry);
    void onPhysicalSizeChanged(const QSizeF &size);
    void onPhysicalDotsPerInchChanged(qreal dpi);
    void onLogicalDotsPerInchChanged(qreal dpi);
    void onVirtualGeometryChanged(const QRect &rect);
    void onPrimaryOrientationChanged(Qt::ScreenOrientation orientation);
    void onOrientationChanged(Qt::ScreenOrientation orientation);
    void onRefreshRateChanged(qreal refreshRate);

protected:
    QScreen *m_screen;

    friend class DesktopBackgroundWindowManager;
};
}

#endif // DESKTOPBACKGROUNDWINDOW_H
