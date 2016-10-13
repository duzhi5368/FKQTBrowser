/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include "fkui_urliconlabel.h"
#include "fkui_urllineedit.h"
#include "fkui_webview.h"
//-----------------------------------------------------
CFKUI_UrlLineEdit::CFKUI_UrlLineEdit(QWidget* pParent)
    : CFKUI_ExLineEdit(pParent)
    , m_pWebView(NULL)
    , m_pIconLabel(NULL)
{
    m_pIconLabel = new CFKUI_UrlIconLabel(this);
    m_pIconLabel->resize(16, 16);
    SetLeftWidget(m_pIconLabel);
    m_DefaultBaseColor = palette().color(QPalette::Base);
}
//-----------------------------------------------------
void CFKUI_UrlLineEdit::SetWebView(CFKUI_WebView* pWebView)
{
    Q_ASSERT(m_pWebView);
    m_pWebView = pWebView;
    m_pIconLabel->m_pWebView = pWebView;
    connect(pWebView, SIGNAL(urlChanged(QUrl)),
        this, SLOT(__SLOT_WebViewUrlChanged(QUrl)));
    connect(pWebView, SIGNAL(iconChanged(QIcon)),
        this, SLOT(__SLOT_WebViewIconChanged(QIcon)));
    connect(pWebView, SIGNAL(loadProgress(int)),
        this, SLOT(update()));
}
//-----------------------------------------------------
QLinearGradient CFKUI_UrlLineEdit::GenerateGradient(const QColor &color) const
{
    QLinearGradient tagGradient(0, 0, 0, height());
    tagGradient.setColorAt(0, m_DefaultBaseColor);
    tagGradient.setColorAt(0.15, color.lighter(120));
    tagGradient.setColorAt(0.5, color);
    tagGradient.setColorAt(0.85, color.lighter(120));
    tagGradient.setColorAt(1, m_DefaultBaseColor);
    return tagGradient;
}
//-----------------------------------------------------
void CFKUI_UrlLineEdit::paintEvent(QPaintEvent* pEvent)
{
    QPalette p = palette();
    if (m_pWebView &&
            m_pWebView->GetUrl().scheme() == QLatin1String("https"))
    {
        QColor tagLightYellowColor(248, 248, 210);
        p.setBrush(QPalette::Base, GenerateGradient(tagLightYellowColor));
    }
    else
    {
        p.setBrush(QPalette::Base, m_DefaultBaseColor);
    }
    setPalette(p);
    CFKUI_ExLineEdit::paintEvent(pEvent);

    QPainter tagPainter(this);
    QStyleOptionFrame tagPanel;
    _InitStyleOption(&tagPanel);
    QRect tagBackgroundRect = style()->subElementRect(QStyle::SE_LineEditContents, &tagPanel, this);
    if (m_pWebView && !hasFocus())
    {
        int nProgress = m_pWebView->GetProgress();
        QColor tagLoadingColor = QColor(116, 192, 250);
        tagPainter.setBrush(GenerateGradient(tagLoadingColor));
        tagPainter.setPen(Qt::transparent);
        int nMid = tagBackgroundRect.width() / 100.0f * nProgress;
        QRect tagProgressRect(tagBackgroundRect.x(), tagBackgroundRect.y(), nMid, tagBackgroundRect.height());
        tagPainter.drawRect(tagProgressRect);
    }
}
//-----------------------------------------------------
void CFKUI_UrlLineEdit::focusOutEvent(QFocusEvent* pEvent)
{
    if (m_pLineEdit->text().isEmpty() && m_pWebView)
        m_pLineEdit->setText(QString::fromUtf8(m_pWebView->GetUrl().toEncoded()));
    CFKUI_ExLineEdit::focusOutEvent(pEvent);
}
//-----------------------------------------------------
void CFKUI_UrlLineEdit::__SLOT_WebViewUrlChanged(const QUrl &url)
{
    m_pLineEdit->setText(QString::fromUtf8(url.toEncoded()));
    m_pLineEdit->setCursorPosition(0);
}
//-----------------------------------------------------
void CFKUI_UrlLineEdit::__SLOT_WebViewIconChanged(const QIcon &icon)
{
    Q_ASSERT(m_pWebView);
    m_pIconLabel->setPixmap(icon.pixmap(16, 16));
}
//-----------------------------------------------------
