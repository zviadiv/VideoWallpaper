#include "desktopvideoplayer.h"
#include "settingsmanager.h"
#include "preferencesdialog.h"
#include "application.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QDir>
#include <QMenu>
#include <QtAV>

static HWND HWORKERW = nullptr;


BOOL CALLBACK EnumWindowsProc(_In_ HWND hwnd, _In_ LPARAM lParam)
{
    Q_UNUSED(lParam)
    HWND defview = FindWindowEx(hwnd, nullptr, TEXT("SHELLDLL_DefView"), nullptr);
    if (defview != nullptr)
    {
        HWORKERW = FindWindowEx(nullptr, hwnd, TEXT("WorkerW"), nullptr);
        if (HWORKERW != nullptr)
            return FALSE;
    }
    return TRUE;
}

HWND getProgman()
{
    return FindWindow(TEXT("Progman"), TEXT("Program Manager"));
}

HWND getWorkerW(bool legacyMode = false)
{
    HWND hwnd = getProgman();
    SendMessage(hwnd, 0x052c, 0, 0);
    EnumWindows(EnumWindowsProc, 0);
    ShowWindow(HWORKERW, legacyMode ? SW_HIDE : SW_SHOW);
    return legacyMode ? hwnd : HWORKERW;
}


DesktopVideoPlayer::DesktopVideoPlayer(QObject *parent)
    : QObject(parent)
{
    int suffixIndex;
    mCurrentVersion = QVersionNumber::fromString(QSysInfo::kernelVersion(), &suffixIndex);

    if (SettingsManager::getInstance()->getAutostart())
        SettingsManager::getInstance()->regAutostart();
    else
        SettingsManager::getInstance()->unregAutostart();
    QtAV::setFFmpegLogLevel("warn");
    QtAV::setLogLevel(QtAV::LogAll);

    createPlayers();

    createSystemTrayMenu();

    /*PreferencesDialog preferencesDialog;

    QObject::connect(&preferencesDialog, &PreferencesDialog::autostartChanged,
        [=](bool enabled)
        {
            if (enabled)
                SettingsManager::getInstance()->regAutostart();
            else
                SettingsManager::getInstance()->unregAutostart();
        });
    QObject::connect(&preferencesDialog, &PreferencesDialog::rendererChanged,
        [=](QtAV::VideoRendererId rendererId) mutable
        {
            if (rendererId != mRenderer->id())
            {
                QtAV::VideoRenderer *newRenderer = QtAV::VideoRenderer::create(rendererId);
                if (!newRenderer || !newRenderer->isAvailable() || !newRenderer->widget())
                {
                    QMessageBox::critical(nullptr, QStringLiteral("Video Wallpaper"), QObject::tr("Current renderer is not available on your platform!"));
                    qApp->quit();
                }
                else
                {
                    if (!mWindowMode)
                    {
                        newRenderer->widget()->setWindowFlags(rendererWindowFlags);
                        // Why is Direct2D image too large?
                        newRenderer->widget()->setGeometry(screenGeometry);
                    }
                    else
                    {
                        newRenderer->widget()->resize(QSize(1280, 720));
                        moveToCenter(newRenderer->widget());
                    }
                    newRenderer->widget()->setWindowIcon(QIcon(QStringLiteral(":/appicon.ico")));
                    newRenderer->widget()->setWindowTitle(QStringLiteral("Dynamic Desktop"));
                    mPlayer->setRenderer(newRenderer);
                    const QtAV::VideoRendererId videoRendererId = newRenderer->id();
                    if (videoRendererId == QtAV::VideoRendererId_GLWidget
                                || videoRendererId == QtAV::VideoRendererId_GLWidget2
                                || videoRendererId == QtAV::VideoRendererId_OpenGLWidget)
                        newRenderer->forcePreferredPixelFormat(true);
                    else
                        newRenderer->forcePreferredPixelFormat(false);
                    if (SettingsManager::getInstance()->getFitDesktop())
                        newRenderer->setOutAspectRatioMode(QtAV::VideoRenderer::RendererAspectRatio);
                    else
                        newRenderer->setOutAspectRatioMode(QtAV::VideoRenderer::VideoAspectRatio);
                    HWND hwnd = nullptr;
                    if (mRenderer->widget())
                    {
                        newRenderer->widget()->setWindowTitle(mRenderer->widget()->windowTitle());
                        if (mRenderer->widget()->geometry() != screenGeometry)
                            newRenderer->widget()->setGeometry(mRenderer->widget()->geometry());
                        if (newRenderer->widget()->isHidden())
                            newRenderer->widget()->show();
                        if (!mWindowMode)
                            hwnd = GetParent(reinterpret_cast<HWND>(mRenderer->widget()->winId()));
                        if (mRenderer->widget()->testAttribute(Qt::WA_DeleteOnClose))
                            mRenderer->widget()->close();
                        else
                        {
                            mRenderer->widget()->close();
                            delete mRenderer->widget();
                        }
                    }
                    else if (newRenderer->widget()->isHidden())
                        newRenderer->widget()->show();
                    if ((hwnd == nullptr) && !mWindowMode)
                    {
                        QVersionNumber win10Version(10, 0, 10240);
                        hwnd = getWorkerW(mCurrentVersion < win10Version);
                    }
                    if ((hwnd != nullptr) && !mWindowMode)
                        SetParent(reinterpret_cast<HWND>(newRenderer->widget()->winId()), hwnd);
                    mRenderer = newRenderer;
                    mainWindow = newRenderer->widget();
                }
            }
        });
    QObject::connect(&preferencesDialog, &PreferencesDialog::videoQualityChanged,
        [=](const QString &quality)
        {
            if (quality == QStringLiteral("default"))
                mRenderer->setQuality(QtAV::VideoRenderer::QualityDefault);
            else if (quality == QStringLiteral("best"))
                mRenderer->setQuality(QtAV::VideoRenderer::QualityBest);
            else
                mRenderer->setQuality(QtAV::VideoRenderer::QualityFastest);
        });

    if (!SettingsManager::getInstance()->getUrl().isEmpty())
    {
        if (mainWindow->isHidden())
            mainWindow->show();
    }*/
}

DesktopVideoPlayer::~DesktopVideoPlayer()
{
    for (int i = 0; i < mPlayers.size(); i++)
        removeVideo(i);
}

void DesktopVideoPlayer::createPlayers()
{
    mRenderers.clear();
    mPlayers.clear();

    for (int i = 0; i < QApplication::screens().size(); i++)
    {
        auto renderer = createVideoRenderer();
        mRenderers.push_back(renderer);

        PlayerPtr player(new QtAV::AVPlayer());
        player->setRenderer(renderer.get());
        player->setRepeat(-1);
        mPlayers.push_back(player);
    }

    if (!mWindowMode)
    {
        QVersionNumber win10Version(10, 0, 10240); // Windows 10 Version 1507
        // How to place our window under desktop icons:
        // Use "Program Manager" as our parent window in Win7/8/8.1.
        // Use "WorkerW" as our parent window in Win10.
        // Use "Program Manager" as our parent window in
        // Win10 is also OK, but our window will come
        // to front if we press "Win + Tab" and it will
        // also block our desktop icons, however using
        // "WorkerW" as our parent window will not result
        // in this problem, I don't know why. It's strange.
        HWND hwnd = getWorkerW(mCurrentVersion < win10Version);
        if (hwnd != nullptr)
            SetParent(reinterpret_cast<HWND>(mPlayers[0]->renderer()->widget()->winId()), hwnd);
    }
}

DesktopVideoPlayer::RendererPtr DesktopVideoPlayer::createVideoRenderer()
{
    auto renderer = QtAV::VideoRenderer::create(SettingsManager::getInstance()->getRenderer());
    if (!renderer || !renderer->isAvailable() || !renderer->widget())
    {
        QMessageBox::critical(nullptr, QStringLiteral("Video Wallpaper"), QObject::tr("Current renderer is not available on your platform!"));
        qApp->quit();
    }
    const QtAV::VideoDecoderId vid = renderer->id();
    if (vid == QtAV::VideoRendererId_GLWidget
            || vid == QtAV::VideoRendererId_GLWidget2
            || vid == QtAV::VideoRendererId_OpenGLWidget)
        renderer->forcePreferredPixelFormat(true);
    else
        renderer->forcePreferredPixelFormat(false);
    QString videoQuality = SettingsManager::getInstance()->getVideoQuality();
    if (videoQuality == QStringLiteral("default"))
        renderer->setQuality(QtAV::VideoRenderer::QualityDefault);
    else if (videoQuality == QStringLiteral("best"))
        renderer->setQuality(QtAV::VideoRenderer::QualityBest);
    else
        renderer->setQuality(QtAV::VideoRenderer::QualityFastest);

    QWidget *mainWindow = renderer->widget();
    const Qt::WindowFlags rendererWindowFlags = Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus;
    const QRect screenGeometry = QApplication::desktop()->screenGeometry(mainWindow);
    if (!mWindowMode)
    {
        mainWindow->setWindowFlags(rendererWindowFlags);
        // Why is Direct2D image too large?
        mainWindow->setGeometry(screenGeometry);
    }
    else
    {
        mainWindow->resize(QSize(1280, 720));
        moveToCenter(mainWindow);
    }
    mainWindow->setWindowIcon(QIcon(QStringLiteral(":/appicon.ico")));
    mainWindow->setWindowTitle(QStringLiteral("Video Wallpaper"));

    return RendererPtr(renderer);
}

void DesktopVideoPlayer::pauseAllPlayers(bool pause)
{
    for (auto &player : mPlayers)
    {
        if (player->isPlaying() == pause)
            player->pause(pause);
    }
}

void DesktopVideoPlayer::muteAllPlayers(bool mute)
{
    for (auto &player : mPlayers)
    {
        if (player->audio())
            player->audio()->setMute(mute);
    }
}

void DesktopVideoPlayer::createSystemTrayMenu()
{
    QMenu trayMenu;

    QAction *removeAction = trayMenu.addAction(QObject::tr("Remove Wallpaper"));
    QObject::connect(removeAction, &QAction::triggered, []
    {

    });

    QAction *changeAction = trayMenu.addAction(QObject::tr("Change Wallpaper"));
    QObject::connect(changeAction, &QAction::triggered, []
    {
        if (!Application::instance()->mainWindowVisible())
        {
            Application::instance()->showMainWindow(true, true);
        }
    });

    trayMenu.addSeparator();
    QAction *playPauseAction = trayMenu.addAction(mPlayers[0]->isPlaying() ?
                QObject::tr("Pause Wallpaper") : QObject::tr("Play Wallpaper"));
    QObject::connect(playPauseAction, &QAction::triggered, [this]
    {
        bool pause = mPlayers[0]->isPlaying();
        pauseAllPlayers(pause);
    });

    QAction *muteAction = trayMenu.addAction(QObject::tr("Mute Wallpaper"));
    muteAction->setCheckable(true);
    QObject::connect(muteAction, &QAction::triggered, [this](bool checked)
    {
        muteAllPlayers(!checked);
    });

    trayMenu.addSeparator();
    trayMenu.addAction(QObject::tr("Quit"), qApp, &QApplication::closeAllWindows);

    QSystemTrayIcon trayIcon;
    trayIcon.setIcon(QIcon(QStringLiteral(":/appicon.ico")));
    trayIcon.setToolTip(QStringLiteral("Video Wallpaper"));
    trayIcon.setContextMenu(&trayMenu);
    trayIcon.show();
    QObject::connect(&trayIcon, &QSystemTrayIcon::activated,
        [=](QSystemTrayIcon::ActivationReason reason)
        {
            if (reason != QSystemTrayIcon::Context)
                changeAction->triggered();
        });
}

void DesktopVideoPlayer::moveToCenter(QWidget *window)
{
    if (!window)
        return;
    unsigned int screenWidth = QApplication::desktop()->screenGeometry(window).width();
    unsigned int screenHeight = QApplication::desktop()->screenGeometry(window).height();
    unsigned int windowWidth = window->width();
    unsigned int windowHeight = window->height();
    unsigned int newX = (screenWidth - windowWidth) / 2;
    unsigned int newY = (screenHeight - windowHeight) / 2;
    window->move(newX, newY);
}


void DesktopVideoPlayer::removeVideo(int screenIndex)
{
    auto &playerInst = mPlayers[screenIndex];

    if (playerInst->isPlaying())
        playerInst->stop();
    playerInst->renderer()->widget()->hide();
    ShowWindow(HWORKERW, SW_HIDE);
}

void DesktopVideoPlayer::playVideo(int screenIndex, const QString &url)
{
    auto &playerInst = mPlayers[screenIndex];

    playerInst->audio()->setVolume(SettingsManager::getInstance()->getVolume());
    playerInst->audio()->setMute(SettingsManager::getInstance()->getMute());

    if (SettingsManager::getInstance()->getHwdec())
    {
        QStringList decoders = SettingsManager::getInstance()->getDecoders();
        if (!decoders.contains(QStringLiteral("FFmpeg")))
            decoders << QStringLiteral("FFmpeg");
        playerInst->setVideoDecoderPriority(decoders);
        if (decoders.contains(QStringLiteral("CUDA")))
        {
            QVariantHash cuda_opt;
            cuda_opt[QStringLiteral("surfaces")] = 0;
            cuda_opt[QStringLiteral("copyMode")] = QStringLiteral("ZeroCopy");
            QVariantHash opt;
            opt[QStringLiteral("CUDA")] = cuda_opt;
            playerInst->setOptionsForVideoCodec(opt);
        }
        if (decoders.contains(QStringLiteral("D3D11")))
        {
            QVariantHash d3d11_opt;
            //d3d11_opt[QStringLiteral("???")] = ???;
            d3d11_opt[QStringLiteral("copyMode")] = QStringLiteral("ZeroCopy");
            QVariantHash opt;
            opt[QStringLiteral("D3D11")] = d3d11_opt;
            playerInst->setOptionsForVideoCodec(opt);
        }
        if (decoders.contains(QStringLiteral("DXVA")))
        {
            QVariantHash dxva_opt;
            //dxva_opt[QStringLiteral("???")] = ???;
            dxva_opt[QStringLiteral("copyMode")] = QStringLiteral("ZeroCopy");
            QVariantHash opt;
            opt[QStringLiteral("DXVA")] = dxva_opt;
            playerInst->setOptionsForVideoCodec(opt);
        }
    }
    else
        playerInst->setVideoDecoderPriority(QStringList() << QStringLiteral("FFmpeg"));


    auto mainWindow = playerInst->renderer()->widget();
    if (mainWindow->isHidden())
        mainWindow->show();

    if (playerInst->isPaused())
        playerInst->pause(false);
    else if (!url.isEmpty())
    {
        playerInst->play(url);
        mainWindow->setWindowTitle(QFileInfo(url).fileName());
    }

    QVersionNumber win10Version(10, 0, 10240);
    bool legacyMode = mCurrentVersion < win10Version;
    ShowWindow(HWORKERW, legacyMode ? SW_HIDE : SW_SHOW);
}

double DesktopVideoPlayer::videoVolume(int screenIndex)
{
    auto &playerInst = mPlayers[screenIndex];
    if (playerInst->audio())
    {
        return playerInst->audio()->volume();
    }
    return 0.0;
}

void DesktopVideoPlayer::setVideoVolume(int screenIndex, double volume)
{
    SettingsManager::getInstance()->setVolume(volume);

    auto &playerInst = mPlayers[screenIndex];
    if (playerInst->audio())
    {
        playerInst->audio()->setVolume(volume);
    }
}

void DesktopVideoPlayer::setMusicVolume(double volume)
{
    //setVideoVolume(volume);
}

bool DesktopVideoPlayer::getMute(int screenIndex) const
{
    auto &playerInst = mPlayers[screenIndex];
    if (playerInst->audio())
    {
        return playerInst->audio()->isMute();
    }
    return true;
}

void DesktopVideoPlayer::setMute(int screenIndex, bool mute)
{
    SettingsManager::getInstance()->setMute(mute);

    auto &playerInst = mPlayers[screenIndex];
    if (playerInst->audio())
    {
        playerInst->audio()->setMute(mute);
        // Restore the volume
        if (!mute)
            playerInst->audio()->setVolume(SettingsManager::getInstance()->getVolume());
    }
}

void DesktopVideoPlayer::playMusic(const QString &url)
{
    //if (mPlayer->isLoaded() && mPlayer->audio())
    //    mPlayer->setExternalAudio(url);
}

void DesktopVideoPlayer::removeMusic()
{
    //mPlayer->setExternalAudio("");
}

void DesktopVideoPlayer::setVideoFillMode(int screenIndex, VideoFillMode mode)
{
    auto &playerInst = mPlayers[screenIndex];
    if (mode == VideoFillMode::Stretch)
        playerInst->renderer()->setOutAspectRatioMode(QtAV::VideoRenderer::RendererAspectRatio);
    else if (mode == VideoFillMode::Contain)
        playerInst->renderer()->setOutAspectRatioMode(QtAV::VideoRenderer::VideoAspectRatio);
    else
    {
        playerInst->renderer()->setOutAspectRatio(3.0);
    }
}

void DesktopVideoPlayer::setScreenMode(ScreenMode mode)
{

}
