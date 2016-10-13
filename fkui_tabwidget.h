/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKUI_TABWIDGET_H
#define FKUI_TABWIDGET_H
//-----------------------------------------------------
#include <QtWidgets/QTabWidget>

#include <QtCore/QUrl>

#include <QWebEnginePage>
#include <QWebEngineFullScreenRequest>
//-----------------------------------------------------
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QLineEdit;
class QCompleter;
class QWebEngineView;
class QStackedWidget;
class QWebEngineProfile;
class QWebEngineDownloadItem;
QT_END_NAMESPACE
//-----------------------------------------------------
class CFKWebActionMapper;
class CFKUI_TabBar;
class CFKUI_WebView;
class CFKUI_FullScreenNotification;
//-----------------------------------------------------
class CFKUI_TabWidget : public QTabWidget
{
    Q_OBJECT
public:
    CFKUI_TabWidget(QWidget* pParent = NULL);
    ~CFKUI_TabWidget();
public:
    void                        Clear();
    void                        AddWebAction(QAction* pAction, QWebEnginePage::WebAction eWebAction);

    QAction*                    GetNewTabAction() const;
    QAction*                    GetCloseTabAction() const;
    QAction*                    GetRecentlyClosedTabsAction() const;
    QAction*                    GetNextTabAction() const;
    QAction*                    GetPreviousTabAction() const;

    QWidget*                    GetLineEditStack() const;
    QLineEdit*                  GetCurrentLineEdit() const;
    CFKUI_WebView*              GetCurrentWebView() const;
    CFKUI_WebView*              GetWebView(int nIndex) const;
    QLineEdit*                  GetLineEdit(int nIndex) const;
    int                         GetWebViewIndex(CFKUI_WebView* pWebView) const;

    QByteArray                  SaveState() const;
    bool                        IsRestoreState(const QByteArray& strState);

    void                        SetProfile(QWebEngineProfile* pProfile);

    void                        SetupPage(QWebEnginePage* pPage);
signals:
    void                        SIGNAL_LoadPage(const QString& url);
    void                        SIGNAL_TabsChanged();
    void                        SIGNAL_LastTabClosed();

    void                        SIGNAL_SetCurrentTitle(const QString& url);
    void                        SIGNAL_ShowStatusBarMessage(const QString& strMessage);
    void                        SIGNAL_LinkHovered(const QString& strLink);
    void                        SIGNAL_LoadProgress(int nProgress);
    void                        SIGNAL_GeometryChangeRequested(const QRect& rectGeometry);
    void                        SIGNAL_MenuBarVisibilityChangeRequested(bool bIsVisible);
    void                        SIGNAL_StatusBarVisibilityChangeRequested(bool bIsVisible);
    void                        SIGNAL_ToolBarVisibilityChangeRequested(bool bIsVisible);
protected:
    void                        mouseDoubleClickEvent(QMouseEvent* pEvent);
    void                        contextMenuEvent(QContextMenuEvent* pEvent);
    void                        mouseReleaseEvent(QMouseEvent* pEvent);
public slots:
    void                        SLOT_LoadUrlInCurrentTab(const QUrl& url);
    CFKUI_WebView*              SLOT_NewTab(bool bIsMakeCurrent = true);
    void                        SLOT_CloneTab(int nIndex = -1);
    void                        SLOT_RequestCloseTab(int nIndex = -1);
    void                        SLOT_CloseTab(int nIndex);
    void                        SLOT_CloseOtherTabs(int nIndex);
    void                        SLOT_ReloadTab(int nIndex = -1);
    void                        SLOT_ReloadAllTabs();
    void                        SLOT_NextTab();
    void                        SLOT_PreviousTab();
    void                        SLOT_SetAudioMutedForTab(int nIndex, bool bIsMute);
private slots:
    void                        __SLOT_CurrentChanged(int nIndex);
    void                        __SLOT_AboutToShowRecentTabsMenu();
    void                        __SLOT_AboutToShowRecentTriggeredAction(QAction* pAction);
    void                        __SLOT_DownloadRequested(QWebEngineDownloadItem* pDownloadItem);
    void                        __SLOT_WebViewLoadStarted();
    void                        __SLOT_WebViewIconChanged(const QIcon& icon);
    void                        __SLOT_WebViewTitleChanged(const QString& title);
    void                        __SLOT_WebViewUrlChanged(const QUrl& url);
    void                        __SLOT_LineEditReturnPressed();
    void                        __SLOT_WindowCloseRequested();
    void                        __SLOT_FullScreenRequested(QWebEngineFullScreenRequest tagRequest);
    void                        __SLOT_WebPageMutedOrAudibleChanged();
    void                        __SLOT_MoveTab(int nFromIndex, int nToIndex);
    void                        __SLOT_HandleTabBarDoubleClicked(int nIndex);
private:
    QAction*                    m_pRecentlyClosedTabsAction;
    QAction*                    m_pNewTabAction;
    QAction*                    m_pCloseTabAction;
    QAction*                    m_pNextTabAction;
    QAction*                    m_pPreviousTabAction;
    QMenu*                      m_pRecentlyClosedTabsMenu;
    QList<QUrl>                 m_ListRecentlyClosedTabs;
    QList<CFKWebActionMapper*>  m_ListActions;
    QCompleter*                 m_pLineEditCompleter;
    QStackedWidget*             m_pLineEdits;
    CFKUI_TabBar*               m_pTabBar;
    QWebEngineProfile*          m_pProfile;
    QWebEngineView*             m_pFullScreenView;
    CFKUI_FullScreenNotification* m_pFullScreenNotification;
};
//-----------------------------------------------------
#endif // FKUI_TABWIDGET_H
