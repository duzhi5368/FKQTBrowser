/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include "fkui_urliconlabel.h"
#include "fkui_webview.h"
//-----------------------------------------------------
CFKUI_UrlIconLabel::CFKUI_UrlIconLabel(QWidget* pParent)
    : QLabel(pParent)
    , m_pWebView( NULL )
{
    setMinimumWidth(16);
    setMinimumHeight(16);
}
//-----------------------------------------------------
void CFKUI_UrlIconLabel::mousePressEvent(QMouseEvent* pEvent)
{
    if (pEvent->button() == Qt::LeftButton)
        m_tagDragStartPos = pEvent->pos();
    QLabel::mousePressEvent(pEvent);
}
//-----------------------------------------------------
void CFKUI_UrlIconLabel::mouseMoveEvent(QMouseEvent* pEvent)
{
    if (pEvent->buttons() == Qt::LeftButton
        && (pEvent->pos() - m_tagDragStartPos).manhattanLength() > QApplication::startDragDistance()
         && m_pWebView)
    {
        QDrag* pDrag = new QDrag(this);
        QMimeData* pMimeData = new QMimeData;
        pMimeData->setText(QString::fromUtf8(m_pWebView->GetUrl().toEncoded()));
        QList<QUrl> ListUrls;
        ListUrls.append(m_pWebView->GetUrl());
        pMimeData->setUrls(ListUrls);
        pDrag->setMimeData(pMimeData);
        pDrag->exec();
    }
}
//-----------------------------------------------------
