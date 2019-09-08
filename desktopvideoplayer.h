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

enum class ScreenMode
{
    Unique = 0, Shared, Copy
};

class DesktopVideoPlayer : public QObject
{
public:
    DesktopVideoPlayer(QObject *parent = nullptr);
    ~DesktopVideoPlayer();

    void removeVideo(int screenIndex);
    void playVideo(int screenIndex, const QString &url);

    double videoVolume(int screenIndex);
    void setVideoVolume(int screenIndex, double volume);

    void setMusicVolume(double volume);

    bool getMute(int screenIndex) const;
    void setMute(int screenIndex, bool mute);

    void playMusic(const QString &url);
    void removeMusic();

    void setVideoFillMode(int screenIndex, VideoFillMode mode);
    void setScreenMode(ScreenMode mode);

private:
    void setDefaultRenderers();
    void setDefaultRenderer(int screenIndex);
    void createSystemTrayMenu();
    void moveToCenter(QWidget *window);

    using PlayerPtr = QSharedPointer<QtAV::AVPlayer>;
    using RendererPtr = QSharedPointer<QtAV::VideoRenderer>;

    void createPlayers();
    RendererPtr createVideoRenderer(const QRect& geometry);
    void pauseAllPlayers(bool pause);
    void muteAllPlayers(bool mute);

    QVector<PlayerPtr> mPlayers;
    QVector<RendererPtr> mRenderers;

    bool mWindowMode = false;

    QVersionNumber mCurrentVersion;
};

#endif // DESKTOPVIDEOPLAYER_H
