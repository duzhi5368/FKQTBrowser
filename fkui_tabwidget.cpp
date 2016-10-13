/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QWebEngineProfile>
#include <QWebEngineDownloadItem>
#include <QWebEngineFullScreenRequest>
#include <QWebEnginePage>

#include <QtGui/QtEvents>
#include <QtGui/QClipboard>

#include <QtWidgets/QLineEdit>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QCompleter>
#include <QtWidgets/QListView>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>
//-----------------------------------------------------
#include "fkwebactionmapper.h"
#include "fkui_tabwidget.h"
#include "fkui_tabbar.h"
#include "fkui_webview.h"
#include "fkui_webpage.h"
#include "fkui_urllineedit.h"
#include "fkui_savepagedialog.h"
#include "fkui_fullscreennotification.h"
#include "fkbrowserapp.h"
#include "fkdownloaddialog.h"
#include "fkhistorycompletionmodel.h"
#include "fkhistoryfiltermodel.h"
#include "fkhistorymgr.h"
//-----------------------------------------------------
static const int    MAX_RECENTLY_CLOSED_TABS_SIZE   = 10;
static const qint32 KEY_TAB_WIDGET_MAGIC            = 0xaa;
static const int    TAB_WIDGET_VERSION              = 1;
//-----------------------------------------------------
CFKUI_TabWidget::CFKUI_TabWidget(QWidget* pParent)
    : QTabWidget(pParent)
    , m_pRecentlyClosedTabsAction(NULL)
    , m_pNewTabAction(NULL)
    , m_pCloseTabAction(NULL)
    , m_pNextTabAction(NULL)
    , m_pPreviousTabAction(NULL)
    , m_pRecentlyClosedTabsMenu(NULL)
    , m_pLineEditCompleter(NULL)
    , m_pLineEdits(NULL)
    , m_pFullScreenView(NULL)
    , m_pFullScreenNotification(NULL)

{
    m_pTabBar       = new CFKUI_TabBar(this);
    m_pProfile      = QWebEngineProfile::defaultProfile();
    m_pLineEdits    = new QStackedWidget(this);

    setElideMode(Qt::ElideRight);
    setDocumentMode(true);

    // TabBar
    connect(m_pTabBar, SIGNAL(SIGNAL_NewTab()),             this, SLOT(SLOT_NewTab()));
    connect(m_pTabBar, SIGNAL(SIGNAL_CloseTab(int)),        this, SLOT(SLOT_RequestCloseTab(int)));
    connect(m_pTabBar, SIGNAL(SIGNAL_CloneTab(int)),        this, SLOT(SLOT_CloneTab(int)));
    connect(m_pTabBar, SIGNAL(SIGNAL_CloseOtherTabs(int)),  this, SLOT(SLOT_CloseOtherTabs(int)));
    connect(m_pTabBar, SIGNAL(SIGNAL_ReloadTab(int)),       this, SLOT(SLOT_ReloadTab(int)));
    connect(m_pTabBar, SIGNAL(SIGNAL_ReloadAllTabs()),      this, SLOT(SLOT_ReloadAllTabs()));
    connect(m_pTabBar, SIGNAL(SIGNAL_MuteTab(int,bool)),    this, SLOT(SLOT_SetAudioMutedForTab(int,bool)));
    connect(m_pTabBar, SIGNAL(tabMoved(int,int)),           this, SLOT(__SLOT_MoveTab(int,int)));
    connect(m_pTabBar, SIGNAL(tabBarDoubleClicked(int)),    this, SLOT(__SLOT_HandleTabBarDoubleClicked(int)));
    setTabBar(m_pTabBar);

    // Actions
    QList<QKeySequence> ListShortcuts;

    m_pNewTabAction = new QAction(QIcon(QLatin1String(":addtab.png")),
                                  tr("New &Tab"), this);
    m_pNewTabAction->setShortcuts(QKeySequence::AddTab);
    m_pNewTabAction->setIconVisibleInMenu(false);
    connect(m_pNewTabAction, SIGNAL(triggered()),
            this, SLOT(SLOT_NewTab()));

    m_pCloseTabAction = new QAction(QIcon(QLatin1String(":closetab.png")),
                                    tr("&Close Tab"), this);
    m_pCloseTabAction->setShortcuts(QKeySequence::Close);
    m_pCloseTabAction->setIconVisibleInMenu(false);
    connect(m_pCloseTabAction, SIGNAL(triggered()),
            this, SLOT(SLOT_RequestCloseTab()));

    m_pNextTabAction = new QAction(tr("Show Next Tab"), this);
    ListShortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BraceRight));
    ListShortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_PageDown));
    ListShortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BracketRight));
    ListShortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Less));
    m_pNextTabAction->setShortcuts(ListShortcuts);
    connect(m_pNextTabAction, SIGNAL(triggered()),
            this, SLOT(SLOT_NextTab()));

    m_pPreviousTabAction = new QAction(tr("Show Previous Tab"), this);
    ListShortcuts.clear();
    ListShortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BraceLeft));
    ListShortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_PageUp));
    ListShortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BracketLeft));
    ListShortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Greater));
    m_pPreviousTabAction->setShortcuts(ListShortcuts);
    connect(m_pPreviousTabAction, SIGNAL(triggered()),
            this, SLOT(SLOT_PreviousTab()));

    m_pRecentlyClosedTabsMenu = new QMenu(this);
    connect(m_pRecentlyClosedTabsMenu, SIGNAL(aboutToShow()),
            this, SLOT(__SLOT_AboutToShowRecentTabsMenu()));
    connect(m_pRecentlyClosedTabsMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(__SLOT_AboutToShowRecentTriggeredAction(QAction*)));

    m_pRecentlyClosedTabsAction = new QAction(tr("Recently Closed Tabs"), this);
    m_pRecentlyClosedTabsAction->setMenu(m_pRecentlyClosedTabsMenu);
    m_pRecentlyClosedTabsAction->setEnabled(false);

    connect(this, SIGNAL(currentChanged(int)),
            this, SLOT(__SLOT_CurrentChanged(int)));
}
//-----------------------------------------------------
CFKUI_TabWidget::~CFKUI_TabWidget()
{
    if(m_pFullScreenNotification)
    {
        delete m_pFullScreenNotification;
        m_pFullScreenNotification = NULL;
    }
    if(m_pFullScreenView)
    {
        delete m_pFullScreenView;
        m_pFullScreenView = NULL;
    }
}
//-----------------------------------------------------
void CFKUI_TabWidget::Clear()
{
    // 清除已关闭的Tabs
    m_ListRecentlyClosedTabs.clear();
    // 清除输入框历史记录
    for (int i = 0; i < m_pLineEdits->count(); ++i)
    {
        QLineEdit* pLineEdit = GetLineEdit(i);
        pLineEdit->setText(pLineEdit->text());
    }
}
//-----------------------------------------------------
void CFKUI_TabWidget::AddWebAction(QAction* pAction, QWebEnginePage::WebAction eWebAction)
{
    if (NULL == pAction)
        return;
    m_ListActions.append(new CFKWebActionMapper(
                             pAction, eWebAction, this));
}
//-----------------------------------------------------
QAction* CFKUI_TabWidget::GetNewTabAction() const
{
    return m_pNewTabAction;
}
//-----------------------------------------------------
QAction* CFKUI_TabWidget::GetCloseTabAction() const
{
    return m_pCloseTabAction;
}
//-----------------------------------------------------
QAction* CFKUI_TabWidget::GetRecentlyClosedTabsAction() const
{
    return m_pRecentlyClosedTabsAction;
}
//-----------------------------------------------------
QAction* CFKUI_TabWidget::GetNextTabAction() const
{
    return m_pNextTabAction;
}
//-----------------------------------------------------
QAction* CFKUI_TabWidget::GetPreviousTabAction() const
{
    return m_pPreviousTabAction;
}
//-----------------------------------------------------
QWidget* CFKUI_TabWidget::GetLineEditStack() const
{
    return m_pLineEdits;
}
//-----------------------------------------------------
QLineEdit* CFKUI_TabWidget::GetCurrentLineEdit() const
{
    return GetLineEdit(m_pLineEdits->currentIndex());
}
//-----------------------------------------------------
CFKUI_WebView* CFKUI_TabWidget::GetCurrentWebView() const
{
    return GetWebView(currentIndex());
}
//-----------------------------------------------------
CFKUI_WebView* CFKUI_TabWidget::GetWebView(int nIndex) const
{
    QWidget* pWidget = this->widget(nIndex);
    if (CFKUI_WebView* pWebView = qobject_cast<CFKUI_WebView*>(pWidget))
    {
        return pWebView;
    }
    else
    {
        // 优化：延迟创建第一个WebView
        if (count() == 1)
        {
            CFKUI_TabWidget *that = const_cast<CFKUI_TabWidget*>(this);
            that->setUpdatesEnabled(false);
            that->SLOT_NewTab();
            that->SLOT_CloseTab(0);
            that->setUpdatesEnabled(true);
            return GetCurrentWebView();
        }
    }
    return NULL;
}
//-----------------------------------------------------
QLineEdit* CFKUI_TabWidget::GetLineEdit(int nIndex) const
{
    CFKUI_UrlLineEdit* pUrlLineEdit = qobject_cast<CFKUI_UrlLineEdit*>(m_pLineEdits->widget(nIndex));
    if (pUrlLineEdit)
    {
        return pUrlLineEdit->GetLineEdit();
    }
    return NULL;
}
//-----------------------------------------------------
int CFKUI_TabWidget::GetWebViewIndex(CFKUI_WebView* pWebView) const
{
    int nIndex = indexOf(pWebView);
    return nIndex;
}
//-----------------------------------------------------
QByteArray CFKUI_TabWidget::SaveState() const
{
    QByteArray  strData;
    QDataStream pStream(&strData, QIODevice::WriteOnly);

    pStream << qint32(KEY_TAB_WIDGET_MAGIC);
    pStream << qint32(TAB_WIDGET_VERSION);

    QStringList ListStrTabs;
    for (int i = 0; i < count(); ++i)
    {
        if (CFKUI_WebView* pTab = qobject_cast<CFKUI_WebView*>(widget(i)))
        {
            ListStrTabs.append(pTab->url().toString());
        }
        else
        {
            ListStrTabs.append(QString::null);
        }
    }
    pStream << ListStrTabs;

    pStream << currentIndex();
    return strData;
}
//-----------------------------------------------------
bool CFKUI_TabWidget::IsRestoreState(const QByteArray& strState)
{
    QByteArray  strData = strState;
    QDataStream pStream(&strData, QIODevice::ReadOnly);
    if (pStream.atEnd())
    {
        return false;
    }

    qint32 nMarker;
    qint32 nVersion;
    pStream >> nMarker;
    pStream >> nVersion;
    if (nMarker != KEY_TAB_WIDGET_MAGIC
            || nVersion != TAB_WIDGET_VERSION)
    {
        return false;
    }

    QStringList ListStrTabs;
    pStream >> ListStrTabs;

    for (int i = 0; i < ListStrTabs.count(); ++i)
    {
        if (i != 0)
        {
            SLOT_NewTab();
        }
        emit SIGNAL_LoadPage(ListStrTabs.at(i));
    }

    int nCurrentTab;
    pStream >> nCurrentTab;
    setCurrentIndex(nCurrentTab);

    return true;
}
//-----------------------------------------------------
void CFKUI_TabWidget::SetProfile(QWebEngineProfile* pProfile)
{
    m_pProfile = pProfile;
    for (int i = 0; i < count(); ++i)
    {
        QWidget *pTabWidget = widget(i);
        if (CFKUI_WebView* pTab = qobject_cast<CFKUI_WebView*>(pTabWidget))
        {
            CFKUI_WebPage* pWebPage = new CFKUI_WebPage(m_pProfile, pTab);
            SetupPage(pWebPage);
            pWebPage->load(pTab->page()->url());
            pTab->setPage(pWebPage);
        }
    }
}
//-----------------------------------------------------
void CFKUI_TabWidget::SetupPage(QWebEnginePage* pPage)
{
    connect(pPage, SIGNAL(windowCloseRequested()),
            this, SLOT(__SLOT_WindowCloseRequested()));
    connect(pPage, SIGNAL(geometryChangeRequested(QRect)),
            this, SIGNAL(SIGNAL_GeometryChangeRequested(QRect)));

#if defined(QWEBENGINEPAGE_MENUBARVISIBILITYCHANGEREQUESTED)
    connect(pPage, SIGNAL(menuBarVisibilityChangeRequested(bool)),
            this, SIGNAL(SIGNAL_MenuBarVisibilityChangeRequested(bool)));
#endif
#if defined(QWEBENGINEPAGE_STATUSBARVISIBILITYCHANGEREQUESTED)
    connect(pPage, SIGNAL(statusBarVisibilityChangeRequested(bool)),
            this, SIGNAL(SIGNAL_StatusBarVisibilityChangeRequested(bool)));
#endif
#if defined(QWEBENGINEPAGE_TOOLBARVISIBILITYCHANGEREQUESTED)
    connect(pPage, SIGNAL(toolBarVisibilityChangeRequested(bool)),
            this, SIGNAL(SIGNAL_ToolBarVisibilityChangeRequested(bool)));
#endif

    // webview actions
    for (int i = 0; i < m_ListActions.count(); ++i)
    {
        CFKWebActionMapper* pMapper = m_ListActions[i];
        pMapper->AddChild(pPage->action(pMapper->GetWebAction()));
    }
}
//-----------------------------------------------------
void CFKUI_TabWidget::mouseDoubleClickEvent(QMouseEvent* pEvent)
{
    if (!childAt(pEvent->pos())
            && pEvent->pos().y() < (tabBar()->y() + tabBar()->height()))
    {
        SLOT_NewTab();
        return;
    }
    QTabWidget::mouseDoubleClickEvent(pEvent);
}
//-----------------------------------------------------
void CFKUI_TabWidget::contextMenuEvent(QContextMenuEvent* pEvent)
{
    if (!childAt(pEvent->pos()))
    {
        m_pTabBar->__SLOT_ContextMenuRequested(pEvent->pos());
        return;
    }
    QTabWidget::contextMenuEvent(pEvent);
}
//-----------------------------------------------------
void CFKUI_TabWidget::mouseReleaseEvent(QMouseEvent* pEvent)
{
    if (pEvent->button() == Qt::MidButton && !childAt(pEvent->pos())
            && pEvent->pos().y() < (tabBar()->y() + tabBar()->height()))
    {
        QUrl url(QApplication::clipboard()->text(QClipboard::Selection));
        if (!url.isEmpty() && url.isValid()
                && !url.scheme().isEmpty())
        {
            CFKUI_WebView* pWebView = SLOT_NewTab();
            pWebView->setUrl(url);
        }
    }
}
//-----------------------------------------------------
void CFKUI_TabWidget::SLOT_LoadUrlInCurrentTab(const QUrl& url)
{
    CFKUI_WebView* pWebView = GetCurrentWebView();
    if (pWebView)
    {
        pWebView->LoadUrl(url);
        pWebView->setFocus();
    }
}
//-----------------------------------------------------
CFKUI_WebView* CFKUI_TabWidget::SLOT_NewTab(bool bIsMakeCurrent)
{   
    // line edit
    CFKUI_UrlLineEdit* pUrlLineEdit = new CFKUI_UrlLineEdit;
    QLineEdit* pLineEdit = pUrlLineEdit->GetLineEdit();
    if (!m_pLineEditCompleter && count() > 0)
    {
        CFKHistoryCompletionModel* pCompletionModel = new CFKHistoryCompletionModel(this);
        pCompletionModel->setSourceModel(CFKBrowserApp::GetHistoryMgrPtr()->GetHistoryFilterModel());
        m_pLineEditCompleter = new QCompleter(pCompletionModel, this);
        QAbstractItemView* pPopUp = m_pLineEditCompleter->popup();
        QListView* pListView = qobject_cast<QListView*>(pPopUp);
        if (pListView)
        {
            pListView->setUniformItemSizes(true);
        }
    }
    pLineEdit->setCompleter(m_pLineEditCompleter);
    connect(pLineEdit, SIGNAL(returnPressed()), this, SLOT(__SLOT_LineEditReturnPressed()));
    m_pLineEdits->addWidget(pUrlLineEdit);
    m_pLineEdits->setSizePolicy(pLineEdit->sizePolicy());


    if (count() == 0)
    {
        QWidget* pEmptyWidget   = new QWidget;
        QPalette pPalette       = pEmptyWidget->palette();
        pPalette.setColor(QPalette::Window, palette().color(QPalette::Base));
        pEmptyWidget->setPalette(pPalette);
        pEmptyWidget->setAutoFillBackground(true);
        disconnect(this, SIGNAL(currentChanged(int)),
            this, SLOT(__SLOT_CurrentChanged(int)));
        addTab(pEmptyWidget, tr("(Untitled)"));
        connect(this, SIGNAL(currentChanged(int)),
            this, SLOT(__SLOT_CurrentChanged(int)));
        return NULL;
    }

    // webview
    CFKUI_WebView *pWebView = new CFKUI_WebView;
    pWebView->setPage(new CFKUI_WebPage(m_pProfile, pWebView));
    pUrlLineEdit->SetWebView(pWebView);
    connect(pWebView, SIGNAL(loadStarted()),
            this, SLOT(__SLOT_WebViewLoadStarted()));
    connect(pWebView, SIGNAL(iconChanged(QIcon)),
            this, SLOT(__SLOT_WebViewIconChanged(QIcon)));
    connect(pWebView, SIGNAL(titleChanged(QString)),
            this, SLOT(__SLOT_WebViewTitleChanged(QString)));
    connect(pWebView->page(), SIGNAL(audioMutedChanged(bool)),
            this, SLOT(__SLOT_WebPageMutedOrAudibleChanged()));
    connect(pWebView->page(), SIGNAL(recentlyAudibleChanged(bool)),
            this, SLOT(__SLOT_WebPageMutedOrAudibleChanged()));
    connect(pWebView, SIGNAL(urlChanged(QUrl)),
            this, SLOT(__SLOT_WebViewUrlChanged(QUrl)));
    addTab(pWebView, tr("(Untitled)"));
    if (bIsMakeCurrent)
    {
        setCurrentWidget(pWebView);
    }

    SetupPage(pWebView->page());

    if (count() == 1)
    {
        __SLOT_CurrentChanged(currentIndex());
    }

    emit SIGNAL_TabsChanged();
    return pWebView;
}
//-----------------------------------------------------
void CFKUI_TabWidget::SLOT_CloneTab(int nIndex)
{
    if (nIndex < 0)
    {
        nIndex = currentIndex();
    }
    if (nIndex < 0 || nIndex >= count())
    {
        return;
    }

    CFKUI_WebView* pTab = SLOT_NewTab(false);
    pTab->setUrl(GetWebView(nIndex)->GetUrl());
}
//-----------------------------------------------------
void CFKUI_TabWidget::SLOT_RequestCloseTab(int nIndex)
{
    if (nIndex < 0)
    {
        nIndex = currentIndex();
    }
    if (nIndex < 0 || nIndex >= count())
    {
        return;
    }

    CFKUI_WebView* pTab = GetWebView(nIndex);
    if (pTab == NULL)
        return;
    pTab->page()->triggerAction(QWebEnginePage::RequestClose);
}
//-----------------------------------------------------
void CFKUI_TabWidget::SLOT_CloseTab(int nIndex)
{
    if (nIndex < 0 || nIndex >= count())
        return;

    bool bIsHasFocus = false;
    if (CFKUI_WebView* pTab = GetWebView(nIndex))
    {
        bIsHasFocus = pTab->hasFocus();

        if (m_pProfile == QWebEngineProfile::defaultProfile())
        {
            m_pRecentlyClosedTabsAction->setEnabled(true);
            m_ListRecentlyClosedTabs.prepend(pTab->url());
            if (m_ListRecentlyClosedTabs.size() >= MAX_RECENTLY_CLOSED_TABS_SIZE)
            {
                m_ListRecentlyClosedTabs.removeLast();
            }
        }
    }

    QWidget* pLineEdit = m_pLineEdits->widget(nIndex);
    m_pLineEdits->removeWidget(pLineEdit);
    pLineEdit->deleteLater();
    QWidget* pWebView = widget(nIndex);
    removeTab(nIndex);
    pWebView->deleteLater();

    emit SIGNAL_TabsChanged();
    if (bIsHasFocus && count() > 0)
    {
        GetCurrentWebView()->setFocus();
    }
    if (count() == 0)
    {
        emit SIGNAL_LastTabClosed();
    }
}
//-----------------------------------------------------
void CFKUI_TabWidget::SLOT_CloseOtherTabs(int nIndex)
{
    if (-1 == nIndex)
        return;

    for (int i = count() - 1; i > nIndex; --i)
    {
        SLOT_CloseTab(i);
    }
    for (int i = nIndex - 1; i >= 0; --i)
    {
        SLOT_CloseTab(i);
    }
}
//-----------------------------------------------------
void CFKUI_TabWidget::SLOT_ReloadTab(int nIndex)
{
    if (nIndex < 0)
    {
        nIndex = currentIndex();
    }
    if (nIndex < 0 || nIndex >= count())
    {
        return;
    }

    QWidget* pWidget = this->widget(nIndex);
    if (CFKUI_WebView* pTab = qobject_cast<CFKUI_WebView*>(pWidget))
    {
        pTab->reload();
    }
}
//-----------------------------------------------------
void CFKUI_TabWidget::SLOT_ReloadAllTabs()
{
    for (int i = 0; i < count(); ++i)
    {
        QWidget* pTabWidget = widget(i);
        if (CFKUI_WebView* pTab = qobject_cast<CFKUI_WebView*>(pTabWidget))
        {
            pTab->reload();
        }
    }
}
//-----------------------------------------------------
void CFKUI_TabWidget::SLOT_NextTab()
{
    int nNext = currentIndex() + 1;
    if (nNext == count())
    {
        nNext = 0;
    }
    setCurrentIndex(nNext);
}
//-----------------------------------------------------
void CFKUI_TabWidget::SLOT_PreviousTab()
{
    int nNext = currentIndex() - 1;
    if (nNext < 0)
        nNext = count() - 1;
    setCurrentIndex(nNext);
}
//-----------------------------------------------------
void CFKUI_TabWidget::SLOT_SetAudioMutedForTab(int nIndex, bool bIsMute)
{
    if (nIndex < 0)
        nIndex = currentIndex();
    if (nIndex < 0 || nIndex >= count())
        return;

    QWidget* pWidget = this->widget(nIndex);
    if (CFKUI_WebView* pWebView = qobject_cast<CFKUI_WebView*>(pWidget))
    {
        pWebView->page()->setAudioMuted(bIsMute);
    }
}
//-----------------------------------------------------
void CFKUI_TabWidget::__SLOT_CurrentChanged(int nIndex)
{
    CFKUI_WebView* pWebView = this->GetWebView(nIndex);
    if (pWebView == NULL)
        return;

    Q_ASSERT(m_pLineEdits->count() == count());

    // 去除老页面信息
    CFKUI_WebView* pOldWebView = this->GetWebView(m_pLineEdits->currentIndex());
    if (pOldWebView != NULL)
    {
        disconnect(pOldWebView->page(), SIGNAL(linkHovered(const QString&)),
                this, SIGNAL(SIGNAL_LinkHovered(const QString&)));
        disconnect(pOldWebView, SIGNAL(loadProgress(int)),
                this, SIGNAL(SIGNAL_LoadProgress(int)));
        disconnect(pOldWebView->page()->profile(), SIGNAL(downloadRequested(QWebEngineDownloadItem*)),
                this, SLOT(__SLOT_DownloadRequested(QWebEngineDownloadItem*)));
        disconnect(pOldWebView->page(), SIGNAL(fullScreenRequested(QWebEngineFullScreenRequest)),
                this, SLOT(__SLOT_FullScreenRequested(QWebEngineFullScreenRequest)));
    }

    // 创建新的槽连接
    connect(pWebView->page(), SIGNAL(linkHovered(const QString&)),
            this, SIGNAL(SIGNAL_LinkHovered(const QString&)));
    connect(pWebView, SIGNAL(loadProgress(int)),
            this, SIGNAL(SIGNAL_LoadProgress(int)));
    connect(pWebView->page()->profile(), SIGNAL(downloadRequested(QWebEngineDownloadItem*)),
            this, SLOT(__SLOT_DownloadRequested(QWebEngineDownloadItem*)));
    connect(pWebView->page(), SIGNAL(fullScreenRequested(QWebEngineFullScreenRequest)),
            this, SLOT(__SLOT_FullScreenRequested(QWebEngineFullScreenRequest)));

    for (int i = 0; i < m_ListActions.count(); ++i)
    {
        CFKWebActionMapper* pMapper = m_ListActions[i];
        pMapper->UpdateCurrent(pWebView->page());
    }
    // 发送消息
    emit SIGNAL_SetCurrentTitle(pWebView->title());
    m_pLineEdits->setCurrentIndex(nIndex);
    emit SIGNAL_LoadProgress(pWebView->GetProgress());
    emit SIGNAL_ShowStatusBarMessage(pWebView->GetLastStatusBarText());

    if (pWebView->GetUrl().isEmpty())
    {
        m_pLineEdits->currentWidget()->setFocus();
    }
    else
    {
        pWebView->setFocus();
    }
}
//-----------------------------------------------------
void CFKUI_TabWidget::__SLOT_AboutToShowRecentTabsMenu()
{
    m_pRecentlyClosedTabsMenu->clear();
    for (int i = 0; i < m_ListRecentlyClosedTabs.count(); ++i)
    {
        QAction* pAction = new QAction(m_pRecentlyClosedTabsMenu);
        pAction->setData(m_ListRecentlyClosedTabs.at(i));
        QIcon icon = CFKBrowserApp::GetAppInstancePtr()->GetIcon(m_ListRecentlyClosedTabs.at(i));
        pAction->setIcon(icon);
        pAction->setText(m_ListRecentlyClosedTabs.at(i).toString());
        m_pRecentlyClosedTabsMenu->addAction(pAction);
    }
}
//-----------------------------------------------------
void CFKUI_TabWidget::__SLOT_AboutToShowRecentTriggeredAction(QAction* pAction)
{
    QUrl url = pAction->data().toUrl();
    SLOT_LoadUrlInCurrentTab(url);
}
//-----------------------------------------------------
void CFKUI_TabWidget::__SLOT_DownloadRequested(QWebEngineDownloadItem* pDownloadItem)
{
    if (pDownloadItem->savePageFormat() != QWebEngineDownloadItem::UnknownSaveFormat)
    {
        CFKUI_SavePageDialog dlg(this, pDownloadItem->savePageFormat(), pDownloadItem->path());
        if (dlg.exec() != CFKUI_SavePageDialog::Accepted)
        {
            return;
        }
        pDownloadItem->setSavePageFormat(dlg.GetPageFormat());
        pDownloadItem->setPath(dlg.GetFilePath());
    }

    CFKBrowserApp::GetDownloadMgrPtr()->SLOT_Download(pDownloadItem);
    pDownloadItem->accept();
}
//-----------------------------------------------------
void CFKUI_TabWidget::__SLOT_WebViewLoadStarted()
{
    CFKUI_WebView* pWebView = qobject_cast<CFKUI_WebView*>(sender());
    int nIndex = GetWebViewIndex(pWebView);
    if (-1 != nIndex)
    {
        QIcon icon(QLatin1String(":loading.gif"));
        setTabIcon(nIndex, icon);
    }
}
//-----------------------------------------------------
void CFKUI_TabWidget::__SLOT_WebViewIconChanged(const QIcon& icon)
{
    CFKUI_WebView *pWebView = qobject_cast<CFKUI_WebView*>(sender());
    int nIndex = GetWebViewIndex(pWebView);
    if (-1 != nIndex)
    {
        setTabIcon(nIndex, icon);
    }
}
//-----------------------------------------------------
void CFKUI_TabWidget::__SLOT_WebViewTitleChanged(const QString& title)
{
    CFKUI_WebView* pWebView = qobject_cast<CFKUI_WebView*>(sender());
    int nIndex = GetWebViewIndex(pWebView);
    if (-1 != nIndex)
    {
        setTabText(nIndex, title);
    }
    if (currentIndex() == nIndex)
    {
        emit SIGNAL_SetCurrentTitle(title);
    }

    CFKBrowserApp::GetHistoryMgrPtr()->UpdateHistoryItem(pWebView->GetUrl(), title);
}
//-----------------------------------------------------
void CFKUI_TabWidget::__SLOT_WebViewUrlChanged(const QUrl& url)
{
    CFKUI_WebView *pWebView = qobject_cast<CFKUI_WebView*>(sender());
    int nIndex = GetWebViewIndex(pWebView);
    if (-1 != nIndex)
    {
        m_pTabBar->setTabData(nIndex, url);
        CFKHistoryMgr* pHistoryManager = CFKBrowserApp::GetHistoryMgrPtr();
        if (url.isValid())
        {
            pHistoryManager->AddHistoryEntry(url.toString());
        }
    }
    emit SIGNAL_TabsChanged();
}
//-----------------------------------------------------
void CFKUI_TabWidget::__SLOT_LineEditReturnPressed()
{
    if (QLineEdit* pLineEdit = qobject_cast<QLineEdit*>(sender()))
    {
        emit SIGNAL_LoadPage(pLineEdit->text());

        if (m_pLineEdits->currentWidget() == pLineEdit)
        {
            GetCurrentWebView()->setFocus();
        }
    }
}
//-----------------------------------------------------
void CFKUI_TabWidget::__SLOT_WindowCloseRequested()
{
    CFKUI_WebPage* pWebPage = qobject_cast<CFKUI_WebPage*>(sender());
    CFKUI_WebView* pWebView = qobject_cast<CFKUI_WebView*>(pWebPage->view());
    int index = GetWebViewIndex(pWebView);
    if (index >= 0)
    {
        SLOT_CloseTab(index);
    }
}
//-----------------------------------------------------
void CFKUI_TabWidget::__SLOT_FullScreenRequested(QWebEngineFullScreenRequest tagRequest)
{
    CFKUI_WebPage* pWebPage = qobject_cast<CFKUI_WebPage*>(sender());
    if (tagRequest.toggleOn())
    {
        if (!m_pFullScreenView)
        {
            m_pFullScreenView = new QWebEngineView();
            m_pFullScreenNotification = new CFKUI_FullScreenNotification(m_pFullScreenView);

            QAction* pExitFullScreenAction = new QAction(m_pFullScreenView);
            pExitFullScreenAction->setShortcut(Qt::Key_Escape);
            connect(pExitFullScreenAction, &QAction::triggered, [pWebPage] {
                pWebPage->triggerAction(QWebEnginePage::ExitFullScreen);
            });
            m_pFullScreenView->addAction(pExitFullScreenAction);
        }
        pWebPage->setView(m_pFullScreenView);
        tagRequest.accept();
        m_pFullScreenView->showFullScreen();
        m_pFullScreenView->raise();
        m_pFullScreenNotification->show();
    }
    else
    {
        if (NULL == m_pFullScreenView)
            return;

        CFKUI_WebView* pOldWebView = this->GetWebView(m_pLineEdits->currentIndex());
        pWebPage->setView(pOldWebView);
        tagRequest.accept();
        delete m_pFullScreenView;
        m_pFullScreenView = NULL;
        window()->hide();
        window()->show();
    }
}
//-----------------------------------------------------
void CFKUI_TabWidget::__SLOT_WebPageMutedOrAudibleChanged()
{
    QWebEnginePage* pWebPage = qobject_cast<QWebEnginePage*>(sender());
    CFKUI_WebView *pWebView = qobject_cast<CFKUI_WebView*>(pWebPage->view());

    int nIndex = GetWebViewIndex(pWebView);
    if (-1 != nIndex)
    {
        QString strTitle    = pWebView->title();

        bool bIsMuted       = pWebPage->isAudioMuted();
        bool bIsAudible     = pWebPage->recentlyAudible();
        if (bIsMuted)
        {
            strTitle += tr(" (muted)");
        }
        else if (bIsAudible)
        {
            strTitle += tr(" (audible)");
        }

        setTabText(nIndex, strTitle);
    }
}
//-----------------------------------------------------
void CFKUI_TabWidget::__SLOT_MoveTab(int nFromIndex, int nToIndex)
{
    QWidget* pLineEdit = m_pLineEdits->widget(nFromIndex);
    m_pLineEdits->removeWidget(pLineEdit);
    m_pLineEdits->insertWidget(nToIndex, pLineEdit);
}
//-----------------------------------------------------
void CFKUI_TabWidget::__SLOT_HandleTabBarDoubleClicked(int nIndex)
{
    if (nIndex != -1)
        return;
    SLOT_NewTab();
}
//-----------------------------------------------------
