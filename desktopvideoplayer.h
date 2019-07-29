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
    void setMusicVolume(double volume);
    void setMute(bool mute);
    void playMusic(const QString &url);
    void removeMusic();

    void setVideoFillMode(VideoFillMode mode);

private:
    void moveToCenter(QWidget *window);

    bool mWindowMode = false;
    QtAV::AVPlayer* mPlayer;
    QtAV::VideoRenderer* mRenderer;
    QVersionNumber mCurrentVersion;
};

#endif // DESKTOPVIDEOPLAYER_H
