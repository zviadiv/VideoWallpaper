#include "application.h"
#include "videowallpaperviewcontroller.h"

#include <QQmlApplicationEngine>
#include <QVersionNumber>
#include <QMessageBox>
#include <QQmlContext>

#include <Windows.h>

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv)
{
    mQmlEngine = new QQmlApplicationEngine(this);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(mQmlEngine, &QQmlApplicationEngine::objectCreated,
                     this, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    mVideoWallpaperVC = new VideoWallpaperViewController(this);

    mQmlEngine->load(url);
}

Application* Application::instance()
{
    return qobject_cast<Application*>(qApp);
}
