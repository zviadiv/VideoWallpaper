#ifndef DESKTOPVIDEOPLAYER_H
#define DESKTOPVIDEOPLAYER_H

#include <QObject>
#include <QFileInfo>
#include <QVersionNumber>

// Forward declare
namespace QtAV
{
    class AVPlayer;
    class VideoRenderer;
}

enum class VideoFillMode
{
    Cover = 0, Contain, Stretch
};

class DesktopVideoPlayer : public QObject
{
public:
    DesktopVideoPlayer(QObject *parent = nullptr);
    ~DesktopVideoPlayer();

    void removeVideo();
    void playVideo(const QString &url);
    void setVideoVolume(double volume);
    void setMute(bool mute);

    void setVideoFillMode(VideoFillMode mode);

private:
    QStringList externalFilesToLoad(const QFileInfo &originalMediaFile, const QString &fileType);
    void moveToCenter(QWidget *window);

    bool mWindowMode = false;
    QtAV::AVPlayer* mPlayer;
    QtAV::VideoRenderer* mRenderer;
    QVersionNumber mCurrentVersion;
};

#endif // DESKTOPVIDEOPLAYER_H
