/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include "fkui_squeezelabel.h"
//-----------------------------------------------------
CFKUI_SqueezeLabel::CFKUI_SqueezeLabel(QWidget* pParent)
    : QLabel(pParent)
{

}
//-----------------------------------------------------
void CFKUI_SqueezeLabel::paintEvent(QPaintEvent* pEvent)
{
    QFontMetrics tagFontMetrics = fontMetrics();
    if (tagFontMetrics.width(text()) > contentsRect().width())
    {
        QString strElided = tagFontMetrics.elidedText(text(), Qt::ElideMiddle, width());
        QString STRoldText = text();
        setText(strElided);
        QLabel::paintEvent(pEvent);
        setText(STRoldText);
    }
    else
    {
        QLabel::paintEvent(pEvent);
    }
}
//-----------------------------------------------------
