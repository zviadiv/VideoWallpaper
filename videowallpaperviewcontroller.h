#ifndef VIDEOWALLPAPERVIEWCONTROLLER_H
#define VIDEOWALLPAPERVIEWCONTROLLER_H

#include "desktopvideoplayer.h"

#include <QObject>
#include <QtDebug>


class VideoWallpaperViewController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString musicUrl MEMBER mMusicUrl NOTIFY musicUrlChanged)
    Q_PROPERTY(float musicVolume READ musicVolume WRITE setMusicVolume NOTIFY musicVolumeChanged)

public:
    explicit VideoWallpaperViewController(QObject *parent = nullptr);

    double musicVolume() const;
    void setMusicVolume(double volume);

signals:
    void musicVolumeChanged();
    void musicUrlChanged();

public slots:
    void playVideo(int screenIndex, const QString &url);
    void removeVideo(int screenIndex);

    void setVideoVolume(int screenIndex, double volume);
    void setMuteVideo(int screenIndex, bool mute);
    void setVideoFillMode(int screenIndex, int mode);

    void setOverlayType(int screenIndex, int type);
    void setOverlayOpacity(int screenIndex, double opacity);

    void setVideoOffset(int screenIndex, double offset);

    void setScreenMode(int mode);

    void playMusic();
    void removeMusic();
    void removeWallpaper();

    void saveWallpaperAndClose();

    void setVideoQuality(int quality);

    bool autoStartEnabled() const;
    void enableAutoStart(bool enable);

private:
    DesktopVideoPlayer *mDesktopPlayer;
    QString mMusicUrl;
    double mMusicVolume;
};

#endif // VIDEOWALLPAPERVIEWCONTROLLER_H
