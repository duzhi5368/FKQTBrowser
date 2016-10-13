/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKBROWSERMAINWINDOW_H
#define FKBROWSERMAINWINDOW_H
//-----------------------------------------------------
#include <QtWidgets/QMainWindow>

#include <QtGui/QIcon>

#include <QtCore/QUrl>
//-----------------------------------------------------
class CFKUI_TabWidget;
class CFKUI_WebView;
class CFKUI_ToolBarSearch;
class CFKUI_ChaseWidget;
class CFKBookmarksToolBar;
class CFKAutoSaver;

QT_BEGIN_NAMESPACE
class QToolBar;
class QAction;
class QMenu;
QT_END_NAMESPACE
//-----------------------------------------------------
class CFKBrowserMainWindow : public QMainWindow
{
    friend class CFKBrowserApp;
    Q_OBJECT
public:
    static const char*      s_strDefaultHome;
public:
    CFKBrowserMainWindow(QWidget* pParent = NULL, Qt::WindowFlags eFlags = 0);
    ~CFKBrowserMainWindow();
public:
    QSize                   sizeHint() const;
public:
    // 获取选项卡管理器
    CFKUI_TabWidget*        GetTabWidget() const;
    CFKUI_WebView*          GetCurrentTab() const;
    QByteArray              SaveState(bool bIsWithTabs = true) const;
    bool                    RestoreState(const QByteArray& state);
    Q_INVOKABLE void        RunScriptOnOpenViews(const QString& source);

public slots:
    // 打开指定Url
    void                    SLOT_LoadPage(const QString& url);
    // 打开Homepage
    void                    SLOT_Home();
protected:
    void                    closeEvent(QCloseEvent *event);
private slots:
    // 打开指定Url
    void                    __SLOT_LoadUrl(const QUrl& url);
    void                    __SLOT_Save();
    void                    __SLOT_LoadProgress(int nProgress);
    void                    __SLOT_UpdateStatusbar(const QString &string);
    void                    __SLOT_UpdateWindowTitle(const QString &title = QString());
    void                    __SLOT_Preferences();
    void                    __SLOT_FileNew();
    void                    __SLOT_FileOpen();
    void                    __SLOT_FilePrintPreview();
    void                    __SLOT_FilePrint();
    void                    __SLOT_FilePrintToPDF();
    void                    __SLOT_PrivateBrowsing();
    void                    __SLOT_FileSaveAs();
    void                    __SLOT_EditFind();
    void                    __SLOT_EditFindNext();
    void                    __SLOT_EditFindPrevious();
    void                    __SLOT_ShowBookmarksDialog();
    void                    __SLOT_AddBookmark();
    void                    __SLOT_ViewZoomIn();
    void                    __SLOT_ViewZoomOut();
    void                    __SLOT_ViewResetZoom();
    void                    __SLOT_ViewToolbar();
    void                    __SLOT_ViewBookmarksBar();
    void                    __SLOT_ViewStatusbar();
    void                    __SLOT_ViewPageSource();
    void                    __SLOT_ViewFullScreen(bool bIsEnable);
    void                    __SLOT_WebSearch();
    void                    __SLOT_ToggleInspector(bool bIsEnable);
    void                    __SLOT_AboutApplication();
    void                    __SLOT_DownloadManager();
    void                    __SLOT_SelectLineEdit();
    void                    __SLOT_AboutToShowBackMenu();
    void                    __SLOT_AboutToShowForwardMenu();
    void                    __SLOT_AboutToShowWindowMenu();
    void                    __SLOT_OpenActionUrl(QAction* pAction);
    void                    __SLOT_ShowWindow();
    void                    __SLOT_SwapFocus();
    void                    __SLOT_HandlePdfPrinted(const QByteArray& result);
    void                    __SLOT_GeometryChangeRequested(const QRect &geometry);
    void                    __SLOT_UpdateToolbarActionText(bool bIsVisible);
    void                    __SLOT_UpdateBookmarksToolbarActionText(bool bIsVisible);
private:
    void                    __LoadDefaultState();
    void                    __SetupMenu();
    void                    __SetupToolBar();
    void                    __UpdateStatusbarActionText(bool bIsVisible);
    void                    __HandleFindTextResult(bool bIsFound);
private:
    QToolBar*               m_pNavigationBar;
    CFKUI_ToolBarSearch*    m_pToolBarSearch;
    CFKUI_TabWidget*        m_pTabWidget;
    CFKUI_ChaseWidget*      m_pChaseWidget;
    CFKBookmarksToolBar*    m_pBookmarksToolBar;
    CFKAutoSaver*           m_pAutoSaver;
    QMenu*                  m_pHistoryBackMenu;
    QMenu*                  m_pHistoryForwardMenu;
    QMenu*                  m_pWindowMenu;
    QAction*                m_pActionHistoryBack;
    QAction*                m_pActionHistoryForward;
    QAction*                m_pActionStop;
    QAction*                m_pActionReload;
    QAction*                m_pActionStopReload;
    QAction*                m_pActionViewToolbar;
    QAction*                m_pActionViewBookmarkBar;
    QAction*                m_pActionViewStatusbar;
    QAction*                m_pActionRestoreLastSession;
    QAction*                m_pActionAddBookmark;
    QIcon                   m_ReloadIcon;
    QIcon                   m_StopIcon;
    QString                 m_strLastSearch;
    QString                 m_strPrinterOutputFileName;
};
//-----------------------------------------------------
#endif // FKBROWSERMAINWINDOW_H
