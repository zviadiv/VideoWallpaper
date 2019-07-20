#ifndef DESKTOPVIDEOPLAYER_H
#define DESKTOPVIDEOPLAYER_H

#include <QObject>
#include <QFileInfo>

// Forward declare
namespace QtAV
{
    class AVPlayer;
    class VideoRenderer;
}

class DesktopVideoPlayer : public QObject
{
public:
    DesktopVideoPlayer(QObject *parent = nullptr);
    ~DesktopVideoPlayer();

    void playVideo(const QString &url);

private:
    QStringList externalFilesToLoad(const QFileInfo &originalMediaFile, const QString &fileType);
    void moveToCenter(QWidget *window);

    bool mWindowMode = false;
    QtAV::AVPlayer* mPlayer;
    QtAV::VideoRenderer* mRenderer;
};

#endif // DESKTOPVIDEOPLAYER_H
