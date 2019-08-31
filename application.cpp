#include "application.h"
#include "videowallpaperviewcontroller.h"

#include <QQmlApplicationEngine>
#include <QVersionNumber>
#include <QMessageBox>
#include <QQmlContext>
#include <QDesktopWidget>

#include <Windows.h>

HANDLE Application::msMutex = nullptr;


Application::Application(int &argc, char **argv)
    : QApplication(argc, argv)
{
    QCoreApplication::setApplicationName(QStringLiteral("Video Wallpaper"));
    QApplication::setApplicationDisplayName(QStringLiteral("Video Wallpaper"));
    QCoreApplication::setApplicationVersion(QStringLiteral("0.1"));
    QCoreApplication::setOrganizationName(QStringLiteral("undefined"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("undefined"));

    checkCompatibility();

    mQmlEngine = new QQmlApplicationEngine(this);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(mQmlEngine, &QQmlApplicationEngine::objectCreated,
                     this, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    QObject::connect(mQmlEngine, &QQmlApplicationEngine::quit, &Application::quit);

    mVideoWallpaperVC = new VideoWallpaperViewController(this);

    mQmlEngine->load(url);
}

Application::~Application()
{
    ReleaseMutex(msMutex);
    CloseHandle(msMutex);
}

Application* Application::instance()
{
    return qobject_cast<Application*>(qApp);
}

bool Application::mainWindowVisible() const
{
    if (!mQmlEngine)
        return false;

    QObject *rootObject = mQmlEngine->rootObjects().first();
    return rootObject->property("visible").toBool();
}

void Application::showMainWindow(bool show, bool center)
{
    if (!mQmlEngine)
        return;

    QObject *rootObject = mQmlEngine->rootObjects().first();
    if (rootObject)
    {
        if (show && center)
        {
            auto screenWidth = QApplication::desktop()->screenGeometry().width();
            auto screenHeight = QApplication::desktop()->screenGeometry().height();

            auto windowWidth = rootObject->property("width").toInt();
            auto windowHeight = rootObject->property("height").toInt();

            auto newX = (screenWidth - windowWidth) / 2;
            auto newY = (screenHeight - windowHeight) / 2;

            rootObject->setProperty("x", newX);
            rootObject->setProperty("y", newY);
        }

        rootObject->setProperty("visible", show);
    }
}

void Application::checkCompatibility()
{
    int suffixIndex;
    QVersionNumber currentVersion = QVersionNumber::fromString(QSysInfo::kernelVersion(), &suffixIndex);
    QVersionNumber win7Version(6, 1, 7600);
    if (currentVersion < win7Version)
    {
        QMessageBox::critical(nullptr, QStringLiteral("Video Wallpaper"), QObject::tr("This application only supports Windows 7 and newer."));
        qApp->quit();
    }
    msMutex = CreateMutex(nullptr, FALSE, TEXT("DesktopVideoPlayer.AppMutex"));
    if ((msMutex != nullptr) && (GetLastError() == ERROR_ALREADY_EXISTS))
    {
        QMessageBox::critical(nullptr, QStringLiteral("Video Wallpaper"), QObject::tr("There is another instance of application running."));
        ReleaseMutex(msMutex);
        qApp->quit();
    }
}
