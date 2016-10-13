/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtGui/QPainter>
//-----------------------------------------------------
#include "fkui_clearbutton.h"
//-----------------------------------------------------
CFKUI_ClearButton::CFKUI_ClearButton(QWidget* pParent)
    : QAbstractButton(pParent)
{
    setCursor(Qt::ArrowCursor);
    setToolTip(tr("Clear"));
    setVisible(false);
    setFocusPolicy(Qt::NoFocus);
}
//-----------------------------------------------------
void CFKUI_ClearButton::paintEvent(QPaintEvent* pEvent)
{
    Q_UNUSED(pEvent);
    QPainter tagPainter(this);
    tagPainter.setRenderHint(QPainter::Antialiasing, true);
    tagPainter.setBrush(isDown()
                     ? palette().color(QPalette::Dark)
                     : palette().color(QPalette::Mid));
    tagPainter.setPen(tagPainter.brush().color());

    int nHeight     = height();
    int nSize       = width();
    int nOffset     = nSize / 5;
    int nRadius     = nSize - nOffset * 2;
    tagPainter.drawEllipse(nOffset, nOffset, nRadius, nRadius);

    tagPainter.setPen(palette().color(QPalette::Base));
    int nBorder = nOffset * 2;
    tagPainter.drawLine(nBorder, nBorder, width() - nBorder, nHeight - nBorder);
    tagPainter.drawLine(nBorder, nHeight - nBorder, width() - nBorder, nBorder);
}
//-----------------------------------------------------
void CFKUI_ClearButton::__SLOT_TextChanged(const QString& strText)
{
    setVisible(!strText.isEmpty());
}
//-----------------------------------------------------
