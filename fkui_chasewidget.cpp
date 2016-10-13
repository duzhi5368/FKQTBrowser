/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtGui/QPainter>

#include <QtWidgets/QApplication>
//-----------------------------------------------------
#include "fkui_chasewidget.h"
//-----------------------------------------------------
CFKUI_ChaseWidget::CFKUI_ChaseWidget(QWidget* pParent,
                                     QPixmap pixmap, bool bIsPixmapEnabled)
    : QWidget(pParent)
    , m_nSegment(0)
    , m_nDelay(0)
    , m_nStep(40)
    , m_nTimerID(-1)
    , m_bIsAnimated(false)
    , m_bIsPixmapEnabled(bIsPixmapEnabled)
    , m_tagPixmap(pixmap)
{

}
//-----------------------------------------------------
void CFKUI_ChaseWidget::SetAnimated(bool bValue)
{
    if (m_bIsAnimated == bValue)
        return;

    m_bIsAnimated = bValue;
    if (m_nTimerID != -1)
    {
        killTimer(m_nTimerID);
        m_nTimerID = -1;
    }
    if (m_bIsAnimated)
    {
        m_nSegment = 0;
        m_nTimerID = startTimer(m_nDelay);
    }

    update();
}
//-----------------------------------------------------
int CFKUI_ChaseWidget::__GetSegmentCount() const
{
    return 360 / m_nStep;
}
//-----------------------------------------------------
QColor CFKUI_ChaseWidget::__ColorForSegment(int nSegment) const
{
    int nIndex  = ((nSegment + m_nSegment) % __GetSegmentCount());
    int nComp   = qMax(0, 255 - (nIndex * (255 / __GetSegmentCount())));
    return QColor(nComp, nComp, nComp, 255);
}
//-----------------------------------------------------
void CFKUI_ChaseWidget::SetPixmapEnabled(bool bEnable)
{
    m_bIsPixmapEnabled = bEnable;
}
//-----------------------------------------------------
QSize CFKUI_ChaseWidget::sizeHint() const
{
    return QSize(32, 32);
}
//-----------------------------------------------------
void CFKUI_ChaseWidget::paintEvent(QPaintEvent* pEvent)
{
    Q_UNUSED(pEvent);

    QPainter p(this);
    if (m_bIsPixmapEnabled && !m_tagPixmap.isNull())
    {
        p.drawPixmap(0, 0, m_tagPixmap);
        return;
    }

    const int nExtent   = qMin(width() - 8, height() - 8);
    const int nDispl    = nExtent / 4;
    const int nExt      = nExtent / 4 - 1;

    p.setRenderHint(QPainter::Antialiasing, true);

    if (m_bIsAnimated)
    {
        p.setPen(Qt::gray);
    }
    else
    {
        p.setPen(QPen(palette().dark().color()));
    }

    p.translate(width() / 2, height() / 2);

    for (int nSegment = 0; nSegment < __GetSegmentCount(); ++nSegment)
    {
        p.rotate(QApplication::isRightToLeft() ? m_nStep : -m_nStep);
        if (m_bIsAnimated)
        {
            p.setBrush(__ColorForSegment(nSegment));
        }
        else
        {
            p.setBrush(palette().background());
        }
        p.drawEllipse(QRect(nDispl, -nExt / 2, nExt, nExt));
    }
}
//-----------------------------------------------------
void CFKUI_ChaseWidget::timerEvent(QTimerEvent* pEvent)
{
    if (pEvent->timerId() == m_nTimerID)
    {
        ++m_nSegment;
        update();
    }
    QWidget::timerEvent(pEvent);
}
//-----------------------------------------------------
