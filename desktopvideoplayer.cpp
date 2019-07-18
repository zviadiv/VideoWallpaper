#include "desktopvideoplayer.h"
#include "settingsmanager.h"
#include "preferencesdialog.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QVersionNumber>
#include <QDir>
#include <QMenu>
#include <QtAV>

static HANDLE mutex = nullptr;
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
    QVersionNumber currentVersion = QVersionNumber::fromString(QSysInfo::kernelVersion(), &suffixIndex);
    QVersionNumber win7Version(6, 1, 7600);
    if (currentVersion < win7Version)
    {
        QMessageBox::critical(nullptr, QStringLiteral("Video Wallpaper"), QObject::tr("This application only supports Windows 7 and newer."));
        qApp->quit();
    }
    mutex = CreateMutex(nullptr, FALSE, TEXT("DesktopVideoPlayer.AppMutex"));
    if ((mutex != nullptr) && (GetLastError() == ERROR_ALREADY_EXISTS))
    {
        QMessageBox::critical(nullptr, QStringLiteral("Video Wallpaper"), QObject::tr("There is another instance running. Please do not run twice."));
        ReleaseMutex(mutex);
        qApp->quit();
    }

    if (SettingsManager::getInstance()->getAutostart())
        SettingsManager::getInstance()->regAutostart();
    else
        SettingsManager::getInstance()->unregAutostart();
    QtAV::setFFmpegLogLevel("warn");
    QtAV::setLogLevel(QtAV::LogAll);
    QtAV::VideoRenderer *renderer = QtAV::VideoRenderer::create(SettingsManager::getInstance()->getRenderer());
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
    if (SettingsManager::getInstance()->getFitDesktop())
        renderer->setOutAspectRatioMode(QtAV::VideoRenderer::RendererAspectRatio);
    else
        renderer->setOutAspectRatioMode(QtAV::VideoRenderer::VideoAspectRatio);
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
    mainWindow->setWindowIcon(QIcon(QStringLiteral(":/bee.ico")));
    mainWindow->setWindowTitle(QStringLiteral("Dynamic Desktop"));
    QtAV::AVPlayer player;
    player.setRenderer(renderer);
    QtAV::SubtitleFilter subtitle;
    subtitle.setPlayer(&player);
    subtitle.installTo(renderer);
    subtitle.setCodec(SettingsManager::getInstance()->getCharset().toLatin1());
    subtitle.setEngines(QStringList() << QStringLiteral("LibASS") << QStringLiteral("FFmpeg"));
    subtitle.setAutoLoad(SettingsManager::getInstance()->getSubtitleAutoLoad());
    subtitle.setEnabled(SettingsManager::getInstance()->getSubtitle());
    player.setRepeat(-1);
    PreferencesDialog preferencesDialog;
    QObject::connect(&player, SIGNAL(positionChanged(qint64)), &preferencesDialog, SIGNAL(updateVideoSlider(qint64)));
    QObject::connect(&player, &QtAV::AVPlayer::loaded,
        [=, &preferencesDialog, &player, &subtitle]
        {
            preferencesDialog.clearAllTracks();
            preferencesDialog.updateVideoSliderUnit(player.notifyInterval());
            preferencesDialog.updateVideoSliderRange(player.duration());
            preferencesDialog.updateVideoSlider(player.position());
            preferencesDialog.setSeekAreaEnabled(player.isSeekable());
            preferencesDialog.setAudioAreaEnabled(player.audio());
            preferencesDialog.updateVideoTracks(player.internalVideoTracks());
            preferencesDialog.updateAudioTracks(player.internalAudioTracks(), false);
            if (SettingsManager::getInstance()->getAudioAutoLoad())
                preferencesDialog.updateAudioTracks(player.externalAudioTracks(), true);
            preferencesDialog.updateSubtitleTracks(player.internalSubtitleTracks(), false);
            if (SettingsManager::getInstance()->getSubtitleAutoLoad())
            {
                QVariantList externalSubtitleTracks;
                QStringList externalSubtitlePaths = externalFilesToLoad(QFileInfo(player.file()), QStringLiteral("sub"));
                if (!externalSubtitlePaths.isEmpty())
                {
                    for (auto& subPath : externalSubtitlePaths)
                    {
                        QVariantMap externalSubtitle;
                        externalSubtitle[QStringLiteral("file")] = subPath;
                        externalSubtitleTracks.append(externalSubtitle);
                    }
                    preferencesDialog.updateSubtitleTracks(externalSubtitleTracks, true);
                }
            }
            if (SettingsManager::getInstance()->getSubtitle())
            {
                subtitle.setEnabled(true);
                if (player.subtitleStreamCount() > 0)
                    player.setSubtitleStream(0);
                else if (SettingsManager::getInstance()->getSubtitleAutoLoad())
                {
                    QStringList externalSubtitles = externalFilesToLoad(QFileInfo(player.file()), QStringLiteral("sub"));
                    if (!externalSubtitles.isEmpty())
                    {
                        if (subtitle.file() != externalSubtitles.constFirst())
                            subtitle.setFile(externalSubtitles.constFirst());
                    }
                    else
                        subtitle.setEnabled(false);
                }
                else
                    subtitle.setEnabled(false);
            }
            else
                subtitle.setEnabled(false);
        });
    QObject::connect(&player, &QtAV::AVPlayer::notifyIntervalChanged,
        [=, &preferencesDialog, &player]
        {
            preferencesDialog.updateVideoSliderUnit(player.notifyInterval());
            preferencesDialog.updateVideoSlider(player.position());
        });
    QObject::connect(&player, &QtAV::AVPlayer::durationChanged,
        [=, &preferencesDialog, &player](qint64 duration)
        {
            preferencesDialog.updateVideoSliderRange(duration);
            preferencesDialog.updateVideoSlider(player.position());
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
        [=, &player]
        {
            if (player.isPlaying())
                player.pause();
        });
    QAction *muteAction = trayMenu.addAction(QObject::tr("Mute"));
    muteAction->setCheckable(true);
    QObject::connect(muteAction, &QAction::triggered,
        [=, &player, &preferencesDialog](bool checked)
        {
            if (player.audio())
            {
                muteAction->setChecked(checked);
                SettingsManager::getInstance()->setMute(checked);
                preferencesDialog.updateVolumeArea();
                player.audio()->setMute(checked);
            }
        });
    trayMenu.addSeparator();
    trayMenu.addAction(QObject::tr("Exit"), qApp, &QApplication::closeAllWindows);
    QSystemTrayIcon trayIcon;
    trayIcon.setIcon(QIcon(QStringLiteral(":/bee.ico")));
    trayIcon.setToolTip(QStringLiteral("Dynamic Desktop"));
    trayIcon.setContextMenu(&trayMenu);
    trayIcon.show();
    if (player.audio())
    {
        player.audio()->setVolume(static_cast<qreal>(SettingsManager::getInstance()->getVolume() / 10.0));
        player.audio()->setMute(SettingsManager::getInstance()->getMute());
        muteAction->setCheckable(true);
        muteAction->setChecked(SettingsManager::getInstance()->getMute());
    }
    else
    {
        muteAction->setCheckable(false);
        muteAction->setEnabled(false);
        preferencesDialog.setVolumeAreaEnabled(false);
    }
    QObject::connect(&trayIcon, &QSystemTrayIcon::activated,
        [=](QSystemTrayIcon::ActivationReason reason)
        {
            if (reason != QSystemTrayIcon::Context)
                optionsAction->triggered();
        });;
    QObject::connect(&preferencesDialog, SIGNAL(pause()), pauseAction, SIGNAL(triggered()));
    QObject::connect(&preferencesDialog, &PreferencesDialog::urlChanged,
        [=, &player](const QString &url)
        {
            if (SettingsManager::getInstance()->getHwdec())
            {
                QStringList decoders = SettingsManager::getInstance()->getDecoders();
                if (!decoders.contains(QStringLiteral("FFmpeg")))
                    decoders << QStringLiteral("FFmpeg");
                player.setVideoDecoderPriority(decoders);
                if (decoders.contains(QStringLiteral("CUDA")))
                {
                    QVariantHash cuda_opt;
                    cuda_opt[QStringLiteral("surfaces")] = 0;
                    cuda_opt[QStringLiteral("copyMode")] = QStringLiteral("ZeroCopy");
                    QVariantHash opt;
                    opt[QStringLiteral("CUDA")] = cuda_opt;
                    player.setOptionsForVideoCodec(opt);
                }
                if (decoders.contains(QStringLiteral("D3D11")))
                {
                    QVariantHash d3d11_opt;
                    //d3d11_opt[QStringLiteral("???")] = ???;
                    d3d11_opt[QStringLiteral("copyMode")] = QStringLiteral("ZeroCopy");
                    QVariantHash opt;
                    opt[QStringLiteral("D3D11")] = d3d11_opt;
                    player.setOptionsForVideoCodec(opt);
                }
                if (decoders.contains(QStringLiteral("DXVA")))
                {
                    QVariantHash dxva_opt;
                    //dxva_opt[QStringLiteral("???")] = ???;
                    dxva_opt[QStringLiteral("copyMode")] = QStringLiteral("ZeroCopy");
                    QVariantHash opt;
                    opt[QStringLiteral("DXVA")] = dxva_opt;
                    player.setOptionsForVideoCodec(opt);
                }
            }
            else
                player.setVideoDecoderPriority(QStringList() << QStringLiteral("FFmpeg"));
            if (mainWindow->isHidden())
                mainWindow->show();
            if (!url.isEmpty())
            {
                player.play(url);
                mainWindow->setWindowTitle(QFileInfo(url).fileName());
            }
            else if (player.isPaused())
                player.pause(false);
        });
    QObject::connect(&preferencesDialog, &PreferencesDialog::volumeChanged,
        [=, &player](unsigned int volume)
        {
            if (player.audio())
                player.audio()->setVolume(static_cast<qreal>(volume / 10.0));
        });
    QObject::connect(&preferencesDialog, &PreferencesDialog::muteChanged,
        [=, &player](bool mute)
        {
            if (player.audio())
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
        [=, &player](qint64 value)
        {
            if (player.isLoaded())
                player.seek(value);
        });
    QObject::connect(&preferencesDialog, &PreferencesDialog::pictureRatioChanged,
        [=](bool fitDesktop)
        {
            if (fitDesktop)
                renderer->setOutAspectRatioMode(QtAV::VideoRenderer::RendererAspectRatio);
            else
                renderer->setOutAspectRatioMode(QtAV::VideoRenderer::VideoAspectRatio);
        });
    QObject::connect(&preferencesDialog, &PreferencesDialog::videoTrackChanged,
        [=, &player](unsigned int id)
        {
            if (player.isLoaded())
                if (id != player.currentVideoStream())
                    player.setVideoStream(id);
        });
    QObject::connect(&preferencesDialog, &PreferencesDialog::audioTrackChanged,
        [=, &player](unsigned int id)
        {
            if (player.isLoaded())
                if (id != player.currentAudioStream())
                    player.setAudioStream(id);
        });
    QObject::connect(&preferencesDialog, &PreferencesDialog::subtitleTrackChanged,
        [=, &player, &subtitle](const QVariant &track)
        {
            if (player.isLoaded())
            {
                const QString newSubFile = track.toString();
                if (QFileInfo::exists(newSubFile) && subtitle.file() != newSubFile)
                    subtitle.setFile(newSubFile);
                else
                {
                    unsigned int id = track.toUInt();
                    if (id != player.currentSubtitleStream())
                        player.setSubtitleStream(id);
                }
            }
        });
    QObject::connect(&preferencesDialog, &PreferencesDialog::subtitleOpened,
        [=, &player, &subtitle](const QString &subPath)
        {
            if (player.isLoaded())
                if (subtitle.file() != subPath)
                    subtitle.setFile(subPath);
        });
    QObject::connect(&preferencesDialog, &PreferencesDialog::audioOpened,
        [=, &player](const QString &audioPath)
        {
            if (player.isLoaded() && player.audio())
                player.setExternalAudio(audioPath);
        });
    QObject::connect(&preferencesDialog, &PreferencesDialog::charsetChanged,
        [=, &subtitle](const QString &charset)
        {
            if (SettingsManager::getInstance()->getSubtitle())
                subtitle.setCodec(charset.toLatin1());
        });
    QObject::connect(&preferencesDialog, &PreferencesDialog::subtitleAutoLoadChanged,
        [=, &subtitle](bool autoload)
        {
            subtitle.setAutoLoad(autoload);
        });
    QObject::connect(&preferencesDialog, &PreferencesDialog::subtitleEnabled,
        [=, &subtitle](bool enabled)
        {
            subtitle.setEnabled(enabled);
        });
    QObject::connect(&preferencesDialog, &PreferencesDialog::rendererChanged,
        [=, &subtitle, &player](QtAV::VideoRendererId rendererId) mutable
        {
            if (rendererId != renderer->id())
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
                    subtitle.uninstall();
                    subtitle.installTo(newRenderer);
                    player.setRenderer(newRenderer);
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
                    if (renderer->widget())
                    {
                        newRenderer->widget()->setWindowTitle(renderer->widget()->windowTitle());
                        if (renderer->widget()->geometry() != screenGeometry)
                            newRenderer->widget()->setGeometry(renderer->widget()->geometry());
                        if (newRenderer->widget()->isHidden())
                            newRenderer->widget()->show();
                        if (!mWindowMode)
                            hwnd = GetParent(reinterpret_cast<HWND>(renderer->widget()->winId()));
                        if (renderer->widget()->testAttribute(Qt::WA_DeleteOnClose))
                            renderer->widget()->close();
                        else
                        {
                            renderer->widget()->close();
                            delete renderer->widget();
                        }
                    }
                    else if (newRenderer->widget()->isHidden())
                        newRenderer->widget()->show();
                    if ((hwnd == nullptr) && !mWindowMode)
                    {
                        QVersionNumber win10Version(10, 0, 10240);
                        hwnd = getWorkerW(currentVersion < win10Version);
                    }
                    if ((hwnd != nullptr) && !mWindowMode)
                        SetParent(reinterpret_cast<HWND>(newRenderer->widget()->winId()), hwnd);
                    renderer = newRenderer;
                    mainWindow = newRenderer->widget();
                }
            }
        });
    QObject::connect(&preferencesDialog, &PreferencesDialog::videoQualityChanged,
        [=](const QString &quality)
        {
            if (quality == QStringLiteral("default"))
                renderer->setQuality(QtAV::VideoRenderer::QualityDefault);
            else if (quality == QStringLiteral("best"))
                renderer->setQuality(QtAV::VideoRenderer::QualityBest);
            else
                renderer->setQuality(QtAV::VideoRenderer::QualityFastest);
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
        HWND hwnd = getWorkerW(currentVersion < win10Version);
        if (hwnd != nullptr)
            SetParent(reinterpret_cast<HWND>(mainWindow->winId()), hwnd);
    }
    //int exec = QApplication::exec();
    ShowWindow(HWORKERW, SW_HIDE);
    ReleaseMutex(mutex);
    CloseHandle(mutex);
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
