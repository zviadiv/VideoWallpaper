#include "videowallpaperviewcontroller.h"
#include "application.h"
#include "settingsmanager.h"

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>

VideoWallpaperViewController::VideoWallpaperViewController(QObject *parent)
    : QObject(parent)
{
    Application::instance()->engine()->rootContext()->setContextProperty("viewController", this);

    mDesktopPlayer = new DesktopVideoPlayer(this);
}

void VideoWallpaperViewController::setVideoVolume(int screenIndex, double volume)
{
    if (screenIndex >= 0)
        mDesktopPlayer->setVideoVolume(screenIndex, volume);
}

void VideoWallpaperViewController::setMuteVideo(int screenIndex, bool mute)
{
    mDesktopPlayer->setMute(screenIndex, mute);
}

void VideoWallpaperViewController::setVideoFillMode(int screenIndex, int mode)
{
    if (screenIndex >= 0)
        mDesktopPlayer->setVideoFillMode(screenIndex, static_cast<VideoFillMode>(mode));
}

void VideoWallpaperViewController::setOverlayType(int screenIndex, int type)
{
    if (screenIndex >= 0)
        mDesktopPlayer->setOverlayType(screenIndex, static_cast<OverlayFilter::OverlayType>(type));
}

void VideoWallpaperViewController::setOverlayOpacity(int screenIndex, double opacity)
{
    if (screenIndex >= 0)
        mDesktopPlayer->setOverlayOpacity(screenIndex, opacity);
}

void VideoWallpaperViewController::setVideoOffset(int screenIndex, double offset)
{
    if (screenIndex >= 0)
        mDesktopPlayer->setVideoOffset(screenIndex, offset);
}

void VideoWallpaperViewController::setScreenMode(int mode)
{
    mDesktopPlayer->setScreenMode(static_cast<ScreenMode>(mode));
}

double VideoWallpaperViewController::musicVolume() const
{
    return mMusicVolume;
}

void VideoWallpaperViewController::setMusicVolume(double volume)
{
    if (volume != mMusicVolume)
    {
        mMusicVolume = volume;
        mDesktopPlayer->setMusicVolume(volume);
        emit musicVolumeChanged();
    }
}

void VideoWallpaperViewController::playVideo(int screenIndex, const QString &url)
{
    if (screenIndex >= 0)
    {
        qDebug() << "Attempting to start video playback: " << url;
        mDesktopPlayer->playVideo(screenIndex, url);
    }
}

void VideoWallpaperViewController::removeVideo(int screenIndex)
{
    if (screenIndex >= 0)
        mDesktopPlayer->removeVideo(screenIndex);
}

void VideoWallpaperViewController::playMusic()
{
    qDebug() << "Attempting to start music playback: " << mMusicUrl;
    // Use music volume settings
    mDesktopPlayer->setMusicVolume(mMusicVolume);
    mDesktopPlayer->playMusic(mMusicUrl);
}

void VideoWallpaperViewController::removeMusic()
{
    // Restore video volume settings
    //mDesktopPlayer->setVideoVolume(mVideoVolume);
    mDesktopPlayer->removeMusic();
}

void VideoWallpaperViewController::removeWallpaper()
{
    mDesktopPlayer->removeAllVideos();
    removeMusic();
}

void VideoWallpaperViewController::saveWallpaperAndClose()
{
    if (Application::instance()->mainWindowVisible())
        Application::instance()->showMainWindow(false, false);
}

void VideoWallpaperViewController::setVideoQuality(int quality)
{
    mDesktopPlayer->setVideoQuality(static_cast<VideoQuality>(quality));
}

bool VideoWallpaperViewController::autoStartEnabled() const
{
    return SettingsManager::getInstance()->getAutostart();
}

void VideoWallpaperViewController::enableAutoStart(bool enable)
{
    if (enable)
        SettingsManager::getInstance()->regAutostart();
    else {
        SettingsManager::getInstance()->unregAutostart();
    }
    SettingsManager::getInstance()->setAutostart(enable);
}
