/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKUI_WEBVIEW_H
#define FKUI_WEBVIEW_H
//-----------------------------------------------------
#include <QWebEngineView>
//-----------------------------------------------------
class CFKUI_WebPage;
//-----------------------------------------------------
class CFKUI_WebView : public QWebEngineView
{
    Q_OBJECT
public:
    CFKUI_WebView(QWidget* pParent = NULL);
public:
    CFKUI_WebPage*  GetWebPage() const;
    void            SetWebPage(CFKUI_WebPage* pPage);
    void            LoadUrl(const QUrl& url);
    QUrl            GetUrl() const;
    QString         GetLastStatusBarText() const;
    int             GetProgress() const;
protected:
    void            mousePressEvent(QMouseEvent* pEvent);
    void            mouseReleaseEvent(QMouseEvent* pEvent);
    void            contextMenuEvent(QContextMenuEvent* pEvent);
    void            wheelEvent(QWheelEvent* pEvent);
private slots:
    void            __SLOT_SetProgress(int nCurProgress);
    void            __SLOT_LoadFinished(bool bIsSuccess);
    void            __SLOT_SetStatusBarText(const QString &string);
    void            __SLOT_OpenLinkInNewTab();
    void            __SLOT_OnFeaturePermissionRequested(const QUrl &securityOrigin,
                                            QWebEnginePage::Feature eFeature);
    void            __SLOT_OnIconChanged(const QIcon &icon);
private:
    QString         m_strStatusBarText;
    QUrl            m_DefaultUrl;
    int             m_nCurProgress;
    CFKUI_WebPage*  m_pWebPage;
};
//-----------------------------------------------------
#endif // FKUI_WEBVIEW_H
