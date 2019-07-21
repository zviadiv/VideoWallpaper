#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

class QQmlApplicationEngine;
class VideoWallpaperViewController;

class Application : public QApplication
{
    Q_OBJECT
public:
    Application(int &argc, char **argv);
    ~Application();

    static Application *instance();

    QQmlApplicationEngine* engine() { return mQmlEngine; }

private:
    void checkCompatibility();

    QQmlApplicationEngine* mQmlEngine;
    VideoWallpaperViewController* mVideoWallpaperVC;

    static Qt::HANDLE msMutex;
};

#endif // APPLICATION_H
