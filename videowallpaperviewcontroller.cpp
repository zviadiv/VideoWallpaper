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
