#include "videowallpaperviewcontroller.h"
#include "application.h"

#include <QQmlApplicationEngine>
#include <QQmlContext>

VideoWallpaperViewController::VideoWallpaperViewController(QObject *parent) : QObject(parent)
{
    Application::instance()->engine()->rootContext()->setContextProperty("viewController", this);
}
