#pragma once

#include "framelesswindow.h"

#include <QtAV>
#include <QtAVWidgets>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public FramelessWindow
{
    Q_OBJECT

signals:
    void pause();
    void muteChanged(bool);
    void volumeChanged(unsigned int);
    void urlChanged(const QString &);
    void autostartChanged(bool);
    void about();
    void seekBySlider(qint64);
    void pictureRatioChanged(bool);
    void videoTrackChanged(unsigned int);
    void audioTrackChanged(unsigned int);
    void subtitleTrackChanged(const QVariant &);
    void charsetChanged(const QString &);
    void subtitleAutoLoadChanged(bool);
    void subtitleEnabled(bool);
    void subtitleOpened(const QString &);
    void audioOpened(const QString &);
    void skinChanged(const QString &);
    void languageChanged(const QString &);
    void rendererChanged(QtAV::VideoRendererId);
    void videoQualityChanged(const QString &);

signals:
    void refreshUi();
    void updateVideoSlider(qint64);
    void updateVideoSliderUnit(int);
    void updateVideoSliderRange(qint64);
    void setVideoAreaEnabled(bool);
    void setSeekAreaEnabled(bool);
    void setAudioAreaEnabled(bool);
    void setVolumeAreaEnabled(bool);
    void updateVolumeArea();
    void updateVideoTracks(const QVariantList &);
    void updateAudioTracks(const QVariantList &, bool);
    void updateSubtitleTracks(const QVariantList &, bool);
    void clearAllTracks();
    void retranslateUI();

public:
    explicit PreferencesDialog(QWidget *parent = nullptr);
    ~PreferencesDialog() override;

protected:
    void showEvent(QShowEvent *event) override;
    void changeEvent(QEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void refreshUI();
    void saveSettings();

private:
    Ui::PreferencesDialog *ui;
    bool closing = false, audioAvailable = true;
    unsigned int sliderUnit = 1000;
};
