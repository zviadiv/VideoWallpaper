#include "application.h"
#include "desktopvideoplayer.h"

#include <QQmlApplicationEngine>
#include <QVersionNumber>
#include <QMessageBox>

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
    mQmlEngine->load(url);

    mDesktopPlayer = new DesktopVideoPlayer(this);
}

Application* Application::instance()
{
    return qobject_cast<Application*>(qApp);
}
