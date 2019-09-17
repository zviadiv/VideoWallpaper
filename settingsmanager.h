#pragma once

#include <QSettings>
#include <QtAV/QtAV>
#include <QtAVWidgets/QtAVWidgets>

class SettingsManager
{
public:
    static SettingsManager *getInstance();

public:
    bool regAutostart();
    void unregAutostart();

    QStringList defaultDecoders() const;

    bool getMute() const;
    double getVolume() const;
    bool getAutostart() const;
    bool getHwdec() const;
    QStringList getDecoders() const;
    QtAV::VideoRendererId getRenderer() const;
    QString getVideoQuality() const;

    void setMute(bool mute);
    void setVolume(double volume);
    void setAutostart(bool enable);
    void setHwdec(bool enable);
    void setDecoders(const QStringList &decoders);
    void setRenderer(QtAV::VideoRendererId vid);
    void setVideoQuality(const QString &quality);

private:
    SettingsManager();
    ~SettingsManager();

private:
    QSettings *settings;
};
