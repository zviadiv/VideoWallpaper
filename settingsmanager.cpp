#include "settingsmanager.h"

#include <QDir>
#include <QUrl>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QCoreApplication>

SettingsManager *SettingsManager::getInstance()
{
    static SettingsManager settingsManager;
    return &settingsManager;
}

void SettingsManager::unregAutostart()
{
    const QString key = QStringLiteral("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
    QSettings set(key, QSettings::NativeFormat);
    if (set.contains(QStringLiteral("Video Wallpaper")))
        set.remove(QStringLiteral("Video Wallpaper"));
}

bool SettingsManager::regAutostart()
{
    unregAutostart();
    const QString key = QStringLiteral("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");
    QSettings set(key, QSettings::NativeFormat);
    if (set.status() != QSettings::NoError)
        return false;
    QString value = QLatin1Char('"') + QCoreApplication::applicationFilePath() + QLatin1Char('"');
    set.setValue(QStringLiteral("Video Wallpaper"), QDir::toNativeSeparators(value));
    return true;
}

QStringList SettingsManager::defaultDecoders() const
{
    return QStringList()
            << QStringLiteral("CUDA")
            << QStringLiteral("D3D11")
            << QStringLiteral("DXVA")
            << QStringLiteral("FFmpeg");
}

bool SettingsManager::getMute() const
{
    return settings->value(QStringLiteral("vw/mute"), false).toBool();
}

double SettingsManager::getVolume() const
{
    double vol = settings->value(QStringLiteral("vw/volume"), 9).toDouble();
    if (vol < 0)
        vol = 0;
    if (vol > 1.0)
        vol = 1.0;
    return vol;
}

bool SettingsManager::getAutostart() const
{
    return settings->value(QStringLiteral("vw/autostart"), false).toBool();
}

bool SettingsManager::getHwdec() const
{
    return settings->value(QStringLiteral("vw/hwdec"), false).toBool();
}

QStringList SettingsManager::getDecoders() const
{
    return settings->value(QStringLiteral("vw/decoders"), defaultDecoders()).toStringList();
}

QtAV::VideoRendererId SettingsManager::getRenderer() const
{
    return settings->value(QStringLiteral("vw/renderer"), QtAV::VideoRendererId_GLWidget2).toInt();
}

QString SettingsManager::getVideoQuality() const
{
    return settings->value(QStringLiteral("vw/quality"), QStringLiteral("fastest")).toString();
}

void SettingsManager::setMute(bool mute)
{
    settings->setValue(QStringLiteral("vw/mute"), mute);
}

void SettingsManager::setVolume(double volume)
{
    double vol = volume;
    if (vol > 1.0)
        vol = 1.0;
    settings->setValue(QStringLiteral("vw/volume"), vol);
}

void SettingsManager::setAutostart(bool enable)
{
    settings->setValue(QStringLiteral("vw/autostart"), enable);
}

void SettingsManager::setHwdec(bool enable)
{
    settings->setValue(QStringLiteral("vw/hwdec"), enable);
}

void SettingsManager::setDecoders(const QStringList &decoders)
{
    settings->setValue(QStringLiteral("vw/decoders"), decoders);
}

void SettingsManager::setRenderer(QtAV::VideoRendererId vid)
{
    settings->setValue(QStringLiteral("vw/renderer"), vid);
}

void SettingsManager::setVideoQuality(const QString &quality)
{
    settings->setValue(QStringLiteral("vw/quality"), quality);
}

SettingsManager::SettingsManager()
{
    QString iniPath = QCoreApplication::applicationDirPath();
    iniPath += QStringLiteral("/config.ini");
    settings = new QSettings(iniPath, QSettings::IniFormat);
}

SettingsManager::~SettingsManager()
{
    delete settings;
}
