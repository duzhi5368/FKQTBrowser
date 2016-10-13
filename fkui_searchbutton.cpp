/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtGui/QPainter>
#include <QtGui/QtEvents>
//-----------------------------------------------------
#include "fkui_searchbutton.h"
//-----------------------------------------------------
CFKUI_SearchButton::CFKUI_SearchButton(QWidget* pParent)
    : QAbstractButton(pParent)
    , m_pMenu(NULL)
{
    setObjectName(QLatin1String("SearchButton"));
    setCursor(Qt::ArrowCursor);
    setFocusPolicy(Qt::NoFocus);
}
//-----------------------------------------------------
void CFKUI_SearchButton::paintEvent(QPaintEvent* pEvent)
{
    Q_UNUSED(pEvent);

    QPainterPath myPath;

    int nRadius = (height() / 5) * 2;
    QRect tagCircle(height() / 3 - 1, height() / 4, nRadius, nRadius);
    myPath.addEllipse(tagCircle);

    myPath.arcMoveTo(tagCircle, 300);
    QPointF c = myPath.currentPosition();
    int nDiff = height() / 7;
    myPath.lineTo(qMin(width() - 2, (int)c.x() + nDiff), c.y() + nDiff);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::darkGray, 2));
    painter.drawPath(myPath);

    if (m_pMenu)
    {
        QPainterPath dropPath;
        dropPath.arcMoveTo(tagCircle, 320);
        QPointF c = dropPath.currentPosition();
        c = QPointF(c.x() + 3.5, c.y() + 0.5);
        dropPath.moveTo(c);
        dropPath.lineTo(c.x() + 4, c.y());
        dropPath.lineTo(c.x() + 2, c.y() + 2);
        dropPath.closeSubpath();
        painter.setPen(Qt::darkGray);
        painter.setBrush(Qt::darkGray);
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.drawPath(dropPath);
    }
    painter.end();
}
//-----------------------------------------------------
void CFKUI_SearchButton::mousePressEvent(QMouseEvent* pEvent)
{
    if (m_pMenu && pEvent->button() == Qt::LeftButton)
    {
        QWidget *p = parentWidget();
        if (p)
        {
            QPoint r = p->mapToGlobal(QPoint(0, p->height()));
            m_pMenu->exec(QPoint(r.x() + height() / 2, r.y()));
        }
        pEvent->accept();
    }
    QAbstractButton::mousePressEvent(pEvent);
}
//-----------------------------------------------------
