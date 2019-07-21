#include "videowallpaperviewcontroller.h"
#include "application.h"
#include "desktopvideoplayer.h"

#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDebug>

VideoWallpaperViewController::VideoWallpaperViewController(QObject *parent) : QObject(parent)
{
    Application::instance()->engine()->rootContext()->setContextProperty("viewController", this);

    mDesktopPlayer = new DesktopVideoPlayer(this);
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
