#ifndef OVERLAYFILTER_H
#define OVERLAYFILTER_H

#include <QtAV/Filter.h>
#include <QtCore/QTime>
#include <QtGui/QMatrix4x4>

class OverlayFilter : public QtAV::VideoFilter
{
    Q_OBJECT
public:
    OverlayFilter(QObject *parent = 0);
    virtual ~OverlayFilter();

    bool isSupported(QtAV::VideoFilterContext::Type ct) const {
        return ct == QtAV::VideoFilterContext::QtPainter || ct == QtAV::VideoFilterContext::X11;
    }

    enum class OverlayType
    {
        None = 0, Black, White
    };

    void setOverlayType(OverlayType type);
    void setOverlayOpacity(double opacity);

    void prepare();

protected:
    virtual void process(QtAV::Statistics* statistics, QtAV::VideoFrame* frame);
    virtual void timerEvent(QTimerEvent *);

private:
    OverlayType mOverlayType;
    double mOverlayOpacity;
};


#endif // OVERLAYFILTER_H
