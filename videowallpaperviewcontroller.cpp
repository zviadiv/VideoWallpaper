#include "videowallpaperviewcontroller.h"
#include "application.h"

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>

VideoWallpaperViewController::VideoWallpaperViewController(QObject *parent)
    : QObject(parent)
{
    Application::instance()->engine()->rootContext()->setContextProperty("viewController", this);

    mDesktopPlayer = new DesktopVideoPlayer(this);
}

double VideoWallpaperViewController::videoVolume(int screenIndex) const
{
    return mDesktopPlayer->videoVolume(screenIndex);
}

void VideoWallpaperViewController::setVideoVolume(int screenIndex, double volume)
{
    mDesktopPlayer->setVideoVolume(screenIndex, volume);
}

bool VideoWallpaperViewController::muteVideo(int screenIndex) const
{
    return mDesktopPlayer->getMute(screenIndex);
}

void VideoWallpaperViewController::setMuteVideo(int screenIndex, bool mute)
{
    mDesktopPlayer->setMute(screenIndex, mute);
}

int VideoWallpaperViewController::videoFillMode(int screenIndex) const
{
    return 0; // mVideoFillMode;
}

void VideoWallpaperViewController::setVideoFillMode(int screenIndex, int mode)
{
    mDesktopPlayer->setVideoFillMode(screenIndex, static_cast<VideoFillMode>(mode));
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
    qDebug() << "Attempting to start video playback: " << url;
    mDesktopPlayer->playVideo(screenIndex, url);
}

void VideoWallpaperViewController::removeVideo(int screenIndex)
{
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
    //removeVideo();
    removeMusic();
}
