/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include "fkui_popupwindow.h"
#include "fkui_webpage.h"
#include "fkui_webview.h"
//-----------------------------------------------------
CFKUI_PopupWindow::CFKUI_PopupWindow(QWebEngineProfile* pProfile)
{
    m_pAddressBar   = new QLineEdit(this);
    m_pWebView      = new CFKUI_WebView(this);

    m_pWebView->setPage(new CFKUI_WebPage(pProfile, m_pWebView));
    QVBoxLayout* pLayout = new QVBoxLayout;
    pLayout->setMargin(0);
    setLayout(pLayout);
    pLayout->addWidget(m_pAddressBar);
    pLayout->addWidget(m_pWebView);
    m_pWebView->setFocus();

    connect(m_pWebView, &CFKUI_WebView::titleChanged,
            this, &QWidget::setWindowTitle);
    connect(m_pWebView, &CFKUI_WebView::urlChanged,
            this, &CFKUI_PopupWindow::__SLOT_SetUrl);
    connect(GetPage(), &CFKUI_WebPage::geometryChangeRequested,
            this, &CFKUI_PopupWindow::__SLOT_AdjustGeometry);
    connect(GetPage(), &CFKUI_WebPage::windowCloseRequested,
            this, &QWidget::close);
}
//-----------------------------------------------------
QWebEnginePage* CFKUI_PopupWindow::GetPage() const
{
    if(m_pWebView == NULL)
        return NULL;
    return m_pWebView->page();
}
//-----------------------------------------------------
void CFKUI_PopupWindow::__SLOT_SetUrl(const QUrl& url)
{
    m_pAddressBar->setText(url.toString());
}
//-----------------------------------------------------
void CFKUI_PopupWindow::__SLOT_AdjustGeometry(const QRect& newGeometry)
{
    const int x1 = frameGeometry().left() - geometry().left();
    const int y1 = frameGeometry().top() - geometry().top();
    const int x2 = frameGeometry().right() - geometry().right();
    const int y2 = frameGeometry().bottom() - geometry().bottom();

    setGeometry(newGeometry.adjusted(x1, y1 - m_pAddressBar->height(), x2, y2));
}
//-----------------------------------------------------
