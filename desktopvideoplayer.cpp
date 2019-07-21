#include "desktopvideoplayer.h"
#include "settingsmanager.h"
#include "preferencesdialog.h"

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
    mRenderer = QtAV::VideoRenderer::create(SettingsManager::getInstance()->getRenderer());
    if (!mRenderer || !mRenderer->isAvailable() || !mRenderer->widget())
    {
        QMessageBox::critical(nullptr, QStringLiteral("Video Wallpaper"), QObject::tr("Current renderer is not available on your platform!"));
        qApp->quit();
    }
    const QtAV::VideoDecoderId vid = mRenderer->id();
    if (vid == QtAV::VideoRendererId_GLWidget
            || vid == QtAV::VideoRendererId_GLWidget2
            || vid == QtAV::VideoRendererId_OpenGLWidget)
        mRenderer->forcePreferredPixelFormat(true);
    else
        mRenderer->forcePreferredPixelFormat(false);
    QString videoQuality = SettingsManager::getInstance()->getVideoQuality();
    if (videoQuality == QStringLiteral("default"))
        mRenderer->setQuality(QtAV::VideoRenderer::QualityDefault);
    else if (videoQuality == QStringLiteral("best"))
        mRenderer->setQuality(QtAV::VideoRenderer::QualityBest);
    else
        mRenderer->setQuality(QtAV::VideoRenderer::QualityFastest);
    /*if (SettingsManager::getInstance()->getFitDesktop())
        mRenderer->setOutAspectRatioMode(QtAV::VideoRenderer::RendererAspectRatio);
    else
        mRenderer->setOutAspectRatioMode(QtAV::VideoRenderer::VideoAspectRatio);*/
    QWidget *mainWindow = mRenderer->widget();
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
    mainWindow->setWindowIcon(QIcon(QStringLiteral(":/bee.ico")));
    mainWindow->setWindowTitle(QStringLiteral("Video Wallpaper"));
    mPlayer = new QtAV::AVPlayer(this);
    mPlayer->setRenderer(mRenderer);
    mPlayer->setRepeat(-1);
    PreferencesDialog preferencesDialog;
    QObject::connect(mPlayer, SIGNAL(positionChanged(qint64)), &preferencesDialog, SIGNAL(updateVideoSlider(qint64)));
    QObject::connect(mPlayer, &QtAV::AVPlayer::loaded,
        [=, &preferencesDialog]
        {
            /*preferencesDialog.clearAllTracks();
            preferencesDialog.updateVideoSliderUnit(mPlayer->notifyInterval());
            preferencesDialog.updateVideoSliderRange(mPlayer->duration());
            preferencesDialog.updateVideoSlider(mPlayer->position());
            preferencesDialog.setSeekAreaEnabled(mPlayer->isSeekable());
            preferencesDialog.setAudioAreaEnabled(mPlayer->audio());
            preferencesDialog.updateVideoTracks(mPlayer->internalVideoTracks());
            preferencesDialog.updateAudioTracks(mPlayer->internalAudioTracks(), false);
            if (SettingsManager::getInstance()->getAudioAutoLoad())
                preferencesDialog.updateAudioTracks(mPlayer->externalAudioTracks(), true);
            */
        });
    QObject::connect(mPlayer, &QtAV::AVPlayer::notifyIntervalChanged,
        [=, &preferencesDialog]
        {
            preferencesDialog.updateVideoSliderUnit(mPlayer->notifyInterval());
            preferencesDialog.updateVideoSlider(mPlayer->position());
        });
    QObject::connect(mPlayer, &QtAV::AVPlayer::durationChanged,
        [=, &preferencesDialog](qint64 duration)
        {
            /*preferencesDialog.updateVideoSliderRange(duration);
            preferencesDialog.updateVideoSlider(mPlayer->position());*/
        });
    QMenu trayMenu;
    QAction *optionsAction = trayMenu.addAction(QObject::tr("Preferences"));
    QObject::connect(optionsAction, &QAction::triggered,
        [=, &preferencesDialog]
        {
            if (preferencesDialog.isHidden())
            {
                moveToCenter(&preferencesDialog);
                preferencesDialog.show();
            }
            if (!preferencesDialog.isActiveWindow())
                preferencesDialog.setWindowState(preferencesDialog.windowState() & ~Qt::WindowMinimized);
            if (!preferencesDialog.isActiveWindow())
            {
                preferencesDialog.raise();
                preferencesDialog.activateWindow();
            }
        });
    trayMenu.addSeparator();
    QAction *playAction = trayMenu.addAction(QObject::tr("Play"));
    QObject::connect(playAction, &QAction::triggered,
        [=, &preferencesDialog]
        {
            preferencesDialog.urlChanged(QString());
        });
    QAction *pauseAction = trayMenu.addAction(QObject::tr("Pause"));
    QObject::connect(pauseAction, &QAction::triggered,
        [=]
        {
            if (mPlayer->isPlaying())
                mPlayer->pause();
        });
    QAction *muteAction = trayMenu.addAction(QObject::tr("Mute"));
    muteAction->setCheckable(true);
    QObject::connect(muteAction, &QAction::triggered,
        [=,&preferencesDialog](bool checked)
        {
            if (mPlayer->audio())
            {
                muteAction->setChecked(checked);
                SettingsManager::getInstance()->setMute(checked);
                preferencesDialog.updateVolumeArea();
                mPlayer->audio()->setMute(checked);
            }
        });
    trayMenu.addSeparator();
    trayMenu.addAction(QObject::tr("Exit"), qApp, &QApplication::closeAllWindows);
    QSystemTrayIcon trayIcon;
    trayIcon.setIcon(QIcon(QStringLiteral(":/bee.ico")));
    trayIcon.setToolTip(QStringLiteral("Video Wallpaper"));
    trayIcon.setContextMenu(&trayMenu);
    trayIcon.show();
    /*if (mPlayer->audio())
    {
        mPlayer->audio()->setVolume(settingsManager::getInstance()->getVolume()));
        mPlayer->audio()->setMute(SettingsManager::getInstance()->getMute());
        muteAction->setCheckable(true);
        muteAction->setChecked(SettingsManager::getInstance()->getMute());
    }
    else
    {
        muteAction->setCheckable(false);
        muteAction->setEnabled(false);
        preferencesDialog.setVolumeAreaEnabled(false);
    }*/
    QObject::connect(&trayIcon, &QSystemTrayIcon::activated,
        [=](QSystemTrayIcon::ActivationReason reason)
        {
            if (reason != QSystemTrayIcon::Context)
                optionsAction->triggered();
        });;
    QObject::connect(&preferencesDialog, SIGNAL(pause()), pauseAction, SIGNAL(triggered()));
    QObject::connect(&preferencesDialog, &PreferencesDialog::volumeChanged,
        [=](unsigned int volume)
        {
            if (mPlayer->audio())
                mPlayer->audio()->setVolume(volume);
        });
    QObject::connect(&preferencesDialog, &PreferencesDialog::muteChanged,
        [=](bool mute)
        {
            if (mPlayer->audio())
                muteAction->triggered(mute);
        });
    QObject::connect(&preferencesDialog, &PreferencesDialog::autostartChanged,
        [=](bool enabled)
        {
            if (enabled)
                SettingsManager::getInstance()->regAutostart();
            else
                SettingsManager::getInstance()->unregAutostart();
        });
    QObject::connect(&preferencesDialog, &PreferencesDialog::seekBySlider,
        [=](qint64 value)
        {
            if (mPlayer->isLoaded())
                mPlayer->seek(value);
        });
    QObject::connect(&preferencesDialog, &PreferencesDialog::pictureRatioChanged,
        [=](bool fitDesktop)
        {
            if (fitDesktop)
                mRenderer->setOutAspectRatioMode(QtAV::VideoRenderer::RendererAspectRatio);
            else
                mRenderer->setOutAspectRatioMode(QtAV::VideoRenderer::VideoAspectRatio);
        });
    QObject::connect(&preferencesDialog, &PreferencesDialog::videoTrackChanged,
        [this](unsigned int id)
        {
            if (mPlayer->isLoaded())
                if (id != mPlayer->currentVideoStream())
                    mPlayer->setVideoStream(id);
        });
    QObject::connect(&preferencesDialog, &PreferencesDialog::audioTrackChanged,
        [=](unsigned int id)
        {
            if (mPlayer->isLoaded())
                if (id != mPlayer->currentAudioStream())
                    mPlayer->setAudioStream(id);
        });
    QObject::connect(&preferencesDialog, &PreferencesDialog::audioOpened,
        [=](const QString &audioPath)
        {
            if (mPlayer->isLoaded() && mPlayer->audio())
                mPlayer->setExternalAudio(audioPath);
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
                    newRenderer->widget()->setWindowIcon(QIcon(QStringLiteral(":/bee.ico")));
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
        preferencesDialog.urlChanged(SettingsManager::getInstance()->getUrl());
    }
    else
        optionsAction->triggered();
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
            SetParent(reinterpret_cast<HWND>(mainWindow->winId()), hwnd);
    }
}

DesktopVideoPlayer::~DesktopVideoPlayer()
{
    removeVideo();
}

void DesktopVideoPlayer::removeVideo()
{
    if (mPlayer->isPlaying())
        mPlayer->stop();
    mRenderer->widget()->hide();
    ShowWindow(HWORKERW, SW_HIDE);
}

void DesktopVideoPlayer::playVideo(const QString &url)
{
    mPlayer->audio()->setVolume(SettingsManager::getInstance()->getVolume());
    mPlayer->audio()->setMute(SettingsManager::getInstance()->getMute());

    if (SettingsManager::getInstance()->getHwdec())
    {
        QStringList decoders = SettingsManager::getInstance()->getDecoders();
        if (!decoders.contains(QStringLiteral("FFmpeg")))
            decoders << QStringLiteral("FFmpeg");
        mPlayer->setVideoDecoderPriority(decoders);
        if (decoders.contains(QStringLiteral("CUDA")))
        {
            QVariantHash cuda_opt;
            cuda_opt[QStringLiteral("surfaces")] = 0;
            cuda_opt[QStringLiteral("copyMode")] = QStringLiteral("ZeroCopy");
            QVariantHash opt;
            opt[QStringLiteral("CUDA")] = cuda_opt;
            mPlayer->setOptionsForVideoCodec(opt);
        }
        if (decoders.contains(QStringLiteral("D3D11")))
        {
            QVariantHash d3d11_opt;
            //d3d11_opt[QStringLiteral("???")] = ???;
            d3d11_opt[QStringLiteral("copyMode")] = QStringLiteral("ZeroCopy");
            QVariantHash opt;
            opt[QStringLiteral("D3D11")] = d3d11_opt;
            mPlayer->setOptionsForVideoCodec(opt);
        }
        if (decoders.contains(QStringLiteral("DXVA")))
        {
            QVariantHash dxva_opt;
            //dxva_opt[QStringLiteral("???")] = ???;
            dxva_opt[QStringLiteral("copyMode")] = QStringLiteral("ZeroCopy");
            QVariantHash opt;
            opt[QStringLiteral("DXVA")] = dxva_opt;
            mPlayer->setOptionsForVideoCodec(opt);
        }
    }
    else
        mPlayer->setVideoDecoderPriority(QStringList() << QStringLiteral("FFmpeg"));


    auto mainWindow = mRenderer->widget();
    if (mainWindow->isHidden())
        mainWindow->show();

    if (mPlayer->isPaused())
        mPlayer->pause(false);
    else if (!url.isEmpty())
    {
        mPlayer->play(url);
        mainWindow->setWindowTitle(QFileInfo(url).fileName());
    }

    QVersionNumber win10Version(10, 0, 10240);
    bool legacyMode = mCurrentVersion < win10Version;
    ShowWindow(HWORKERW, legacyMode ? SW_HIDE : SW_SHOW);
}

void DesktopVideoPlayer::setVideoVolume(double volume)
{
    SettingsManager::getInstance()->setVolume(volume);

    if (mPlayer->audio())
    {
        mPlayer->audio()->setVolume(volume);
    }
}

void DesktopVideoPlayer::setMusicVolume(double volume)
{
    setVideoVolume(volume);
}

void DesktopVideoPlayer::setMute(bool mute)
{
    SettingsManager::getInstance()->setMute(mute);

    if (mPlayer->audio())
    {
        mPlayer->audio()->setMute(mute);
        // Restore the volume
        if (!mute)
            mPlayer->audio()->setVolume(SettingsManager::getInstance()->getVolume());
    }
}

void DesktopVideoPlayer::playMusic(const QString &url)
{
    if (mPlayer->isLoaded() && mPlayer->audio())
        mPlayer->setExternalAudio(url);
}

void DesktopVideoPlayer::removeMusic()
{
    mPlayer->setExternalAudio("");
}

void DesktopVideoPlayer::setVideoFillMode(VideoFillMode mode)
{
    if (mode == VideoFillMode::Stretch)
        mRenderer->setOutAspectRatioMode(QtAV::VideoRenderer::RendererAspectRatio);
    else if (mode == VideoFillMode::Contain)
        mRenderer->setOutAspectRatioMode(QtAV::VideoRenderer::VideoAspectRatio);
    else
    {
        mRenderer->setOutAspectRatio(3.0);
    }
}

QStringList DesktopVideoPlayer::externalFilesToLoad(const QFileInfo &originalMediaFile, const QString &fileType)
{
    if (!originalMediaFile.exists() || originalMediaFile.isDir() || fileType.isEmpty())
        return QStringList();
    QDir subDir(originalMediaFile.absoluteDir());
    QFileInfoList fileList = subDir.entryInfoList(QDir::Files | QDir::NoSymLinks, QDir::Name);
    if (fileList.count() < 2)
        return QStringList();
    const QString fileBaseName = originalMediaFile.baseName().toLower();
    QStringList newFileList;
    for (auto& fi : fileList)
    {
        if (fi.absoluteFilePath() == originalMediaFile.absoluteFilePath())
            continue;
        const QString newBaseName = fi.baseName().toLower();
        if (newBaseName == fileBaseName)
            if (fileType.toLower() == QLatin1String("sub"))
            {
                if (fi.suffix().toLower() == QLatin1String("ass")
                        || fi.suffix().toLower() == QLatin1String("ssa")
                        || fi.suffix().toLower() == QLatin1String("srt")
                        || fi.suffix().toLower() == QLatin1String("sub"))
                    newFileList.append(QDir::toNativeSeparators(fi.absoluteFilePath()));
            }
            else if (fileType.toLower() == QLatin1String("audio"))
                if (fi.suffix().toLower() == QLatin1String("mka"))
                    newFileList.append(QDir::toNativeSeparators(fi.absoluteFilePath()));
    }
    return newFileList;
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
