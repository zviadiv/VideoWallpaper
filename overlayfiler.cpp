#include "overlayfilter.h"
#include <QWidget>
#include <math.h>

using namespace QtAV;

OverlayFilter::OverlayFilter(QObject *parent):
    VideoFilter(parent),
    mOverlayType(OverlayType::None),
    mOverlayOpacity(0.5)
{
}

OverlayFilter::~OverlayFilter()
{
}

void OverlayFilter::setOverlayType(OverlayType type)
{
    mOverlayType = type;
}

void OverlayFilter::setOverlayOpacity(double opacity)
{
    mOverlayOpacity = opacity;
}

void OverlayFilter::prepare()
{
    /*VideoFilterContext *ctx = static_cast<VideoFilterContext*>(context());
    if (!mText.isEmpty()) {
        ctx->font.setPixelSize(32);
        ctx->font.setBold(true);
        if (!mCanRot)
            return;
        mMat.translate(ctx->rect.center().x(), 0, 0);
    } else if (!mImage.isNull()) {
        if (!mCanRot)
            return;
        mMat.translate(ctx->rect.x() + mImage.width()/2, 0, 0);
    }*/
}

void OverlayFilter::timerEvent(QTimerEvent *)
{
    if (qobject_cast<QWidget*>(parent()))
        ((QWidget*)parent())->update();
}

void OverlayFilter::process(Statistics *statistics, VideoFrame *frame)
{
    Q_UNUSED(statistics);
    Q_UNUSED(frame);
    if (!isEnabled())
        return;

    VideoFilterContext *ctx = static_cast<VideoFilterContext*>(context());
    //ctx->transform = mMat.toTransform();

    if (mOverlayType != OverlayType::None)
    {
        QColor color = mOverlayType == OverlayType::Black ? Qt::black : Qt::white;
        color.setAlphaF(mOverlayOpacity);
        ctx->painter->fillRect(ctx->rect, color);
    }
}

