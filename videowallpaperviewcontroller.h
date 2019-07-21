#ifndef VIDEOWALLPAPERVIEWCONTROLLER_H
#define VIDEOWALLPAPERVIEWCONTROLLER_H

#include <QObject>
#include <QtDebug>

class DesktopVideoPlayer;

class VideoWallpaperViewController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString videoUrl MEMBER mVideoUrl NOTIFY videoUrlChanged)
    Q_PROPERTY(QString musicUrl MEMBER mMusicUrl NOTIFY musicUrlChanged)

public:
    explicit VideoWallpaperViewController(QObject *parent = nullptr);

signals:
    void videoUrlChanged();
    void musicUrlChanged();

public slots:
    void playVideo();
    void removeVideo();

private:
    DesktopVideoPlayer *mDesktopPlayer;
    QString mVideoUrl;
    QString mMusicUrl;
};

#endif // VIDEOWALLPAPERVIEWCONTROLLER_H
