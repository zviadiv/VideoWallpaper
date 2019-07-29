#ifndef VIDEOWALLPAPERVIEWCONTROLLER_H
#define VIDEOWALLPAPERVIEWCONTROLLER_H

#include "desktopvideoplayer.h"

#include <QObject>
#include <QtDebug>


class VideoWallpaperViewController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString videoUrl MEMBER mVideoUrl NOTIFY videoUrlChanged)
    Q_PROPERTY(QString musicUrl MEMBER mMusicUrl NOTIFY musicUrlChanged)
    Q_PROPERTY(float videoVolume READ videoVolume WRITE setVideoVolume NOTIFY videoVolumeChanged)
    Q_PROPERTY(float musicVolume READ musicVolume WRITE setMusicVolume NOTIFY musicVolumeChanged)
    Q_PROPERTY(bool mute READ mute WRITE setMute NOTIFY muteChanged)
    Q_PROPERTY(int videoFillMode READ videoFillMode WRITE setVideoFillMode NOTIFY videoFillModeChanged)

public:
    explicit VideoWallpaperViewController(QObject *parent = nullptr);

    double videoVolume() const;
    void setVideoVolume(double volume);

    double musicVolume() const;
    void setMusicVolume(double volume);

    bool mute() const;
    void setMute(bool mute);

    int videoFillMode() const;
    void setVideoFillMode(int mode);

signals:
    void videoUrlChanged();
    void videoVolumeChanged();
    void musicVolumeChanged();
    void musicUrlChanged();
    void muteChanged();
    void videoFillModeChanged();

public slots:
    void playVideo();
    void removeVideo();
    void playMusic();
    void removeMusic();
    void removeWallpaper();

private:
    DesktopVideoPlayer *mDesktopPlayer;
    QString mVideoUrl;
    QString mMusicUrl;
    double mVideoVolume;
    double mMusicVolume;
    bool mMute;
    int mVideoFillMode;
};

#endif // VIDEOWALLPAPERVIEWCONTROLLER_H
