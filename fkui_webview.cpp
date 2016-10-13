/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtWidgets/QMenu>
#include <QtWidgets/QApplication>

#include <QtGui/QtEvents>
#include <QtGui/QClipboard>

#include <QtCore/QTimer>

#include <QWebEngineContextMenuData>
//-----------------------------------------------------
#include "fkui_webview.h"
#include "fkui_webpage.h"
#include "fkui_featurepermissionbar.h"
#include "fkbrowserapp.h"
//-----------------------------------------------------
CFKUI_WebView::CFKUI_WebView(QWidget* pParent)
    : QWebEngineView(pParent)
    , m_nCurProgress(0)
    , m_pWebPage( NULL )
{
    connect(this, SIGNAL(loadProgress(int)),
            this, SLOT(__SLOT_SetProgress(int)));
    connect(this, SIGNAL(loadFinished(bool)),
            this, SLOT(__SLOT_LoadFinished(bool)));

    // 闭包
    connect(this, &QWebEngineView::renderProcessTerminated,
            [=](QWebEnginePage::RenderProcessTerminationStatus eTermStatus, int nStatusCode)
    {
        const char* strStatus = "";
        switch (eTermStatus)
        {
        case QWebEnginePage::NormalTerminationStatus:
            strStatus = "(normal exit)";
            break;
        case QWebEnginePage::AbnormalTerminationStatus:
            strStatus = "(abnormal exit)";
            break;
        case QWebEnginePage::CrashedTerminationStatus:
            strStatus = "(crashed)";
            break;
        case QWebEnginePage::KilledTerminationStatus:
            strStatus = "(killed)";
            break;
        }

        qInfo() << "Render process exited with code" << nStatusCode << strStatus;
        QTimer::singleShot(0, [this] { reload(); });
    });
}
//-----------------------------------------------------
CFKUI_WebPage* CFKUI_WebView::GetWebPage() const
{
    return m_pWebPage;
}
//-----------------------------------------------------
void CFKUI_WebView::SetWebPage(CFKUI_WebPage* pPage)
{
    // 销毁旧Page
    if(m_pWebPage)
    {
        m_pWebPage->deleteLater();
    }
    // 设置新Page
    m_pWebPage = pPage;
    QWebEngineView::setPage(pPage);
    // 创建新槽
    connect(page(), SIGNAL(statusBarMessage(QString)),
            SLOT(__SLOT_SetStatusBarText(QString)));
    disconnect(page(), &QWebEnginePage::iconChanged,
               this, &CFKUI_WebView::__SLOT_OnIconChanged);
    connect(page(), SIGNAL(iconChanged(QIcon)),
            this, SLOT(__SLOT_OnIconChanged(QIcon)));
    connect(page(), &CFKUI_WebPage::featurePermissionRequested,
            this, &CFKUI_WebView::__SLOT_OnFeaturePermissionRequested);
}
//-----------------------------------------------------
void CFKUI_WebView::LoadUrl(const QUrl& url)
{
    m_DefaultUrl = url;
    load(url);
}
//-----------------------------------------------------
QUrl CFKUI_WebView::GetUrl() const
{
    QUrl url = QWebEngineView::url();
    if (!url.isEmpty())
        return url;

    return m_DefaultUrl;
}
//-----------------------------------------------------
QString CFKUI_WebView::GetLastStatusBarText() const
{
    return m_strStatusBarText;
}
//-----------------------------------------------------
int CFKUI_WebView::GetProgress() const
{
    return m_nCurProgress;
}
//-----------------------------------------------------
void CFKUI_WebView::mousePressEvent(QMouseEvent* pEvent)
{
    m_pWebPage->m_ePressedButtons = pEvent->buttons();
    m_pWebPage->m_eKeyboardModifiers = pEvent->modifiers();
    QWebEngineView::mousePressEvent(pEvent);
}
//-----------------------------------------------------
void CFKUI_WebView::mouseReleaseEvent(QMouseEvent* pEvent)
{
    QWebEngineView::mouseReleaseEvent(pEvent);
    if (!pEvent->isAccepted() && (m_pWebPage->m_ePressedButtons & Qt::MidButton))
    {
        QUrl url(QApplication::clipboard()->text(QClipboard::Selection));
        if (!url.isEmpty() && url.isValid() && !url.scheme().isEmpty())
        {
            setUrl(url);
        }
    }
}
//-----------------------------------------------------
void CFKUI_WebView::contextMenuEvent(QContextMenuEvent* pEvent)
{
    QMenu* pMenu;
    if (page()->contextMenuData().linkUrl().isValid())
    {
        pMenu = new QMenu(this);
        pMenu->addAction(page()->action(QWebEnginePage::OpenLinkInThisWindow));
        pMenu->addAction(page()->action(QWebEnginePage::OpenLinkInNewWindow));
        pMenu->addAction(page()->action(QWebEnginePage::OpenLinkInNewBackgroundTab));
        pMenu->addSeparator();
        pMenu->addAction(page()->action(QWebEnginePage::DownloadLinkToDisk));
        pMenu->addAction(page()->action(QWebEnginePage::CopyLinkToClipboard));
    }
    else
    {
        pMenu = page()->createStandardContextMenu();
    }
    if (page()->contextMenuData().selectedText().isEmpty())
    {
        pMenu->addAction(page()->action(QWebEnginePage::SavePage));
    }
    connect(pMenu, &QMenu::aboutToHide, pMenu, &QObject::deleteLater);
    pMenu->popup(pEvent->globalPos());
}
//-----------------------------------------------------
void CFKUI_WebView::wheelEvent(QWheelEvent* pEvent)
{
    /*
    if (QApplication::keyboardModifiers() & Qt::ControlModifier)
    {
        int nNumDegrees = pEvent->delta() / 8;
        int pNumSteps = nNumDegrees / 15;
        setTextSizeMultiplier(textSizeMultiplier() + pNumSteps * 0.1);
        pEvent->accept();
        return;
    }
    */

    QWebEngineView::wheelEvent(pEvent);
}
//-----------------------------------------------------
void  CFKUI_WebView::__SLOT_SetProgress(int nCurProgress)
{
    m_nCurProgress = nCurProgress;
}
//-----------------------------------------------------
void  CFKUI_WebView::__SLOT_LoadFinished(bool bIsSuccess)
{
    if (bIsSuccess && 100 != m_nCurProgress)
    {
        qWarning() << "Received finished signal while progress is still:" << GetProgress()
                   << "Url:" << GetUrl();
    }
    m_nCurProgress = 0;
}
//-----------------------------------------------------
void  CFKUI_WebView::__SLOT_SetStatusBarText(const QString &string)
{
    m_strStatusBarText = string;
}
//-----------------------------------------------------
void  CFKUI_WebView::__SLOT_OpenLinkInNewTab()
{
    pageAction(QWebEnginePage::OpenLinkInNewTab)->trigger();
}
//-----------------------------------------------------
void  CFKUI_WebView::__SLOT_OnFeaturePermissionRequested(
        const QUrl &securityOrigin, QWebEnginePage::Feature eFeature)
{
    CFKUI_FeaturePermissionBar* pPermissionBar = new CFKUI_FeaturePermissionBar(this);
    connect(pPermissionBar, &CFKUI_FeaturePermissionBar::SIGNAL_FeaturePermissionProvided,
            page(), &QWebEnginePage::setFeaturePermission);
    connect(page(), &QWebEnginePage::loadStarted,
            pPermissionBar, &QObject::deleteLater);

    pPermissionBar->RequestPermission(securityOrigin, eFeature);
}
//-----------------------------------------------------
void  CFKUI_WebView::__SLOT_OnIconChanged(const QIcon &icon)
{
    if (icon.isNull())
        emit iconChanged(CFKBrowserApp::GetAppInstancePtr()->GetDefaultIcon());
    else
        emit iconChanged(icon);
}
//-----------------------------------------------------
