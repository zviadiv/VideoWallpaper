#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

class QQmlApplicationEngine;
class DesktopVideoPlayer;

class Application : public QApplication
{
    Q_OBJECT
public:
    Application(int &argc, char **argv);

    static Application *instance();

    QQmlApplicationEngine* engine() { return mQmlEngine; }

private:
    QQmlApplicationEngine* mQmlEngine;
    DesktopVideoPlayer *mDesktopPlayer;
};

#endif // APPLICATION_H
