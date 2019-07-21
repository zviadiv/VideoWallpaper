#include "application.h"
#include "videowallpaperviewcontroller.h"

#include <QQmlApplicationEngine>
#include <QVersionNumber>
#include <QMessageBox>
#include <QQmlContext>

#include <Windows.h>

HANDLE Application::msMutex = nullptr;


Application::Application(int &argc, char **argv)
    : QApplication(argc, argv)
{
    QCoreApplication::setApplicationName(QStringLiteral("Video Wallpaper"));
    QApplication::setApplicationDisplayName(QStringLiteral("Video Wallpaper"));
    QCoreApplication::setApplicationVersion(QStringLiteral("1.0"));
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
        QMessageBox::critical(nullptr, QStringLiteral("Video Wallpaper"), QObject::tr("There is another instance running. Please do not run twice."));
        ReleaseMutex(msMutex);
        qApp->quit();
    }
}
