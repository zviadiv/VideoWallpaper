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
    setMute(true);
}

double VideoWallpaperViewController::videoVolume() const
{
    return mVideoVolume;
}

void VideoWallpaperViewController::setVideoVolume(double volume)
{
    if (volume != mVideoVolume)
    {
        mVideoVolume = volume;
        mDesktopPlayer->setVideoVolume(volume);
        emit videoVolumeChanged();
    }
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

bool VideoWallpaperViewController::mute() const
{
    return mMute;
}

void VideoWallpaperViewController::setMute(bool mute)
{
    if (mute != mMute)
    {
        mMute = mute;
        mDesktopPlayer->setMute(mute);
        emit muteChanged();
    }
}

int VideoWallpaperViewController::videoFillMode() const
{
    return mVideoFillMode;
}

void VideoWallpaperViewController::setVideoFillMode(int mode)
{
    if (mode != mVideoFillMode)
    {
        mVideoFillMode = mode;
        mDesktopPlayer->setVideoFillMode(static_cast<VideoFillMode>(mode));
        emit videoFillModeChanged();
    }
}

void VideoWallpaperViewController::playVideo()
{
    qDebug() << "Attempting to start video playback: " << mVideoUrl;
    mDesktopPlayer->playVideo(mVideoUrl);
}

void VideoWallpaperViewController::removeVideo()
{
    mDesktopPlayer->removeVideo();
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
    mDesktopPlayer->setVideoVolume(mVideoVolume);
    mDesktopPlayer->removeMusic();
}

void VideoWallpaperViewController::removeWallpaper()
{
    removeVideo();
    removeMusic();
}
