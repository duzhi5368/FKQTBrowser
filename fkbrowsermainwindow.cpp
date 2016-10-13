/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QShortcut>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStyle>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QPlainTextEdit>

#include <QtCore/QSettings>
#include <QtCore/QDebug>

#include <QtPrintSupport/QPrintPreviewDialog>

#include <QWebEngineSettings>
#include <QWebEngineHistoryItem>
//-----------------------------------------------------
#include "fkautosaver.h"
#include "fkbrowsermainwindow.h"
#include "fkbrowserapp.h"
#include "fkbookmarksmodel.h"
#include "fkbookmarksmgr.h"
#include "fkbookmarkstoolbar.h"
#include "fkbookmarksmenu.h"
#include "fkbookmarksdialog.h"
#include "fkaddbookmarkdialog.h"
#include "fkdownloaddialog.h"
#include "fkhistorymenu.h"
#include "fksettingdialog.h"
#include "fkprinttopdfdialog.h"
#include "fkui_chasewidget.h"
#include "fkui_toolbarsearch.h"
#include "fkui_webview.h"
#include "fkui_tabwidget.h"
//-----------------------------------------------------
static const qint32 BROWSER_MAIN_WINDOW_MAGIC = 0xbb;
static const qint32 BROWSER_MAIN_WINDOW_VERSION = 1;
//-----------------------------------------------------
template<typename Arg, typename R, typename C>
struct TInvokeWrapper
{
    R *receiver;
    void (C::*memberFun)(Arg);
    void operator()(Arg result)
    {
        (receiver->*memberFun)(result);
    }
};
template<typename Arg, typename R, typename C>
TInvokeWrapper<Arg, R, C> Invoke(R *receiver, void (C::*memberFun)(Arg))
{
    TInvokeWrapper<Arg, R, C> wrapper = {receiver, memberFun};
    return wrapper;
}
//-----------------------------------------------------
const char *CFKBrowserMainWindow::s_strDefaultHome = "http://www.google.com.ph/";
//-----------------------------------------------------
CFKBrowserMainWindow::CFKBrowserMainWindow(
        QWidget* pParent, Qt::WindowFlags eFlags)
    : QMainWindow(pParent, eFlags)
    , m_pActionHistoryBack(NULL)
    , m_pActionHistoryForward(NULL)
    , m_pActionStop(NULL)
    , m_pActionReload(NULL)
{
    setToolButtonStyle(Qt::ToolButtonFollowStyle);
    setAttribute(Qt::WA_DeleteOnClose, true);
    statusBar()->setSizeGripEnabled(true);
    __SetupMenu();
    __SetupToolBar();

    QWidget* pCentralWidget             = new QWidget(this);
    CFKBookmarksModel* pBookmarksModel  = CFKBrowserApp::GetBookmarksMgrPtr()->GetBookmarksModel();
    m_pBookmarksToolBar                 = new CFKBookmarksToolBar(pBookmarksModel, this);
    connect(m_pBookmarksToolBar, SIGNAL(SIGNAL_OpenUrl(QUrl)),
            m_pTabWidget, SLOT(SLOT_LoadUrlInCurrentTab(QUrl)));
    connect(m_pBookmarksToolBar->toggleViewAction(), SIGNAL(toggled(bool)),
            this, SLOT(__SLOT_UpdateBookmarksToolbarActionText(bool)));

    QVBoxLayout* pLayout = new QVBoxLayout;
    pLayout->setSpacing(0);
    pLayout->setMargin(0);
    addToolBarBreak();
    addToolBar(m_pBookmarksToolBar);
    pLayout->addWidget(m_pTabWidget);
    pCentralWidget->setLayout(pLayout);
    setCentralWidget(pCentralWidget);


    connect(m_pTabWidget, SIGNAL(SIGNAL_LoadPage(QString)),
            this, SLOT(SLOT_LoadPage(QString)));
    connect(m_pTabWidget, SIGNAL(SIGNAL_SetCurrentTitle(QString)),
            this, SLOT(__SLOT_UpdateWindowTitle(QString)));
    connect(m_pTabWidget, SIGNAL(SIGNAL_ShowStatusBarMessage(QString)),
            statusBar(), SLOT(showMessage(QString)));
    connect(m_pTabWidget, SIGNAL(SIGNAL_LinkHovered(QString)),
            statusBar(), SLOT(showMessage(QString)));
    connect(m_pTabWidget, SIGNAL(SIGNAL_LoadProgress(int)),
            this, SLOT(__SLOT_LoadProgress(int)));
    connect(m_pTabWidget, SIGNAL(SIGNAL_TabsChanged()),
            m_pAutoSaver, SLOT(SLOT_OnChange()));
    connect(m_pTabWidget, SIGNAL(SIGNAL_GeometryChangeRequested(QRect)),
            this, SLOT(__SLOT_GeometryChangeRequested(QRect)));
    connect(m_pTabWidget, SIGNAL(SIGNAL_MenuBarVisibilityChangeRequested(bool)),
            menuBar(), SLOT(setVisible(bool)));
    connect(m_pTabWidget, SIGNAL(SIGNAL_StatusBarVisibilityChangeRequested(bool)),
            statusBar(), SLOT(setVisible(bool)));
    connect(m_pTabWidget, SIGNAL(SIGNAL_ToolBarVisibilityChangeRequested(bool)),
            m_pNavigationBar, SLOT(setVisible(bool)));
    connect(m_pTabWidget, SIGNAL(SIGNAL_ToolBarVisibilityChangeRequested(bool)),
            m_pBookmarksToolBar, SLOT(setVisible(bool)));
    connect(m_pTabWidget, SIGNAL(SIGNAL_LastTabClosed()),
            m_pTabWidget, SLOT(SLOT_NewTab()));


    __SLOT_UpdateWindowTitle();
    __LoadDefaultState();
    m_pTabWidget->SLOT_NewTab();

    int nSize = m_pTabWidget->GetLineEditStack()->sizeHint().height();
    m_pNavigationBar->setIconSize(QSize(nSize, nSize));
}
//-----------------------------------------------------
CFKBrowserMainWindow::~CFKBrowserMainWindow()
{
    m_pAutoSaver->SLOT_OnChange();
    m_pAutoSaver->SaveIfNeccessary();
}
//-----------------------------------------------------
QSize CFKBrowserMainWindow::sizeHint() const
{
    QRect desktopRect = QApplication::desktop()->screenGeometry();
    QSize size = desktopRect.size() * qreal(0.9);
    return size;
}
//-----------------------------------------------------
CFKUI_TabWidget* CFKBrowserMainWindow::GetTabWidget() const
{
    return m_pTabWidget;
}
//-----------------------------------------------------
CFKUI_WebView* CFKBrowserMainWindow::GetCurrentTab() const
{
    return m_pTabWidget->GetCurrentWebView();
}
//-----------------------------------------------------
QByteArray CFKBrowserMainWindow::SaveState(bool bIsWithTabs) const
{
    QByteArray strData;
    QDataStream stream(&strData, QIODevice::WriteOnly);

    stream << qint32(BROWSER_MAIN_WINDOW_MAGIC);
    stream << qint32(BROWSER_MAIN_WINDOW_VERSION);

    stream << size();
    stream << !m_pNavigationBar->isHidden();
    stream << !m_pBookmarksToolBar->isHidden();
    stream << !statusBar()->isHidden();
    if (bIsWithTabs)
    {
        stream << GetTabWidget()->SaveState();
    }
    else
    {
        stream << QByteArray();
    }
    return strData;
}
//-----------------------------------------------------
bool CFKBrowserMainWindow::RestoreState(const QByteArray& state)
{
    QByteArray strSd = state;
    QDataStream stream(&strSd, QIODevice::ReadOnly);
    if (stream.atEnd())
        return false;

    qint32      marker;
    qint32      v;
    stream      >> marker;
    stream      >> v;
    if (marker != BROWSER_MAIN_WINDOW_MAGIC || v != BROWSER_MAIN_WINDOW_VERSION)
        return false;

    QSize       size;
    bool        bIsShowToolbar;
    bool        bIsShowBookmarksBar;
    bool        bIsShowStatusbar;
    QByteArray  strTabState;

    stream      >> size;
    stream      >> bIsShowToolbar;
    stream      >> bIsShowBookmarksBar;
    stream      >> bIsShowStatusbar;
    stream      >> strTabState;

    resize(size);

    m_pNavigationBar->setVisible(bIsShowToolbar);
    __SLOT_UpdateToolbarActionText(bIsShowToolbar);

    m_pBookmarksToolBar->setVisible(bIsShowBookmarksBar);
    __SLOT_UpdateBookmarksToolbarActionText(bIsShowBookmarksBar);

    statusBar()->setVisible(bIsShowStatusbar);
    __UpdateStatusbarActionText(bIsShowStatusbar);

    if (!GetTabWidget()->IsRestoreState(strTabState))
    {
        return false;
    }

    return true;
}
//-----------------------------------------------------
void CFKBrowserMainWindow::RunScriptOnOpenViews(const QString& source)
{
    for (int i =0; i < GetTabWidget()->count(); ++i)
    {
        GetTabWidget()->GetWebView(i)->page()->runJavaScript(source);
    }
}
//-----------------------------------------------------
// 打开指定Url SLOT
void CFKBrowserMainWindow::SLOT_LoadPage(const QString& url)
{
    __SLOT_LoadUrl(QUrl::fromUserInput(url));
}
//-----------------------------------------------------
// 打开Homepage
void CFKBrowserMainWindow::SLOT_Home()
{
    QString strHomepage = CFKBrowserApp::GetAppInstancePtr()->GetHomepageUrl();
    SLOT_LoadPage(strHomepage);
}
//-----------------------------------------------------
void CFKBrowserMainWindow::closeEvent(QCloseEvent *event)
{
    if (m_pTabWidget->count() > 1) {
        int ret = QMessageBox::warning(this, QString(),
                           tr("Are you sure you want to close the window?"
                              "  There are %1 tabs open").arg(m_pTabWidget->count()),
                           QMessageBox::Yes | QMessageBox::No,
                           QMessageBox::No);
        if (ret == QMessageBox::No) {
            event->ignore();
            return;
        }
    }
    event->accept();
    deleteLater();
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_LoadUrl(const QUrl& url)
{
    if (!GetCurrentTab() || !url.isValid())
        return;

    m_pTabWidget->GetCurrentLineEdit()->setText(QString::fromUtf8(url.toEncoded()));
    m_pTabWidget->SLOT_LoadUrlInCurrentTab(url);
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_Save()
{
    CFKBrowserApp::GetAppInstancePtr()->SaveSession();

    QSettings settings;
    settings.beginGroup(QLatin1String("BrowserMainWindow"));
    QByteArray data = saveState(false);
    settings.setValue(QLatin1String("defaultState"), data);
    settings.endGroup();
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_LoadProgress(int nProgress)
{
    if (nProgress < 100 && nProgress > 0)
    {
        m_pChaseWidget->SetAnimated(true);
        disconnect(m_pActionStopReload, SIGNAL(triggered()),
                   m_pActionReload, SLOT(trigger()));
        if (m_StopIcon.isNull())
            m_StopIcon = style()->standardIcon(QStyle::SP_BrowserStop);
        m_pActionStopReload->setIcon(m_StopIcon);
        connect(m_pActionStopReload, SIGNAL(triggered()),
                m_pActionStop, SLOT(trigger()));
        m_pActionStopReload->setToolTip(tr("Stop loading the current page"));
    }
    else
    {
        m_pChaseWidget->SetAnimated(false);
        disconnect(m_pActionStopReload, SIGNAL(triggered()),
                   m_pActionStop, SLOT(trigger()));
        m_pActionStopReload->setIcon(m_ReloadIcon);
        connect(m_pActionStopReload, SIGNAL(triggered()),
                m_pActionReload, SLOT(trigger()));
        m_pActionStopReload->setToolTip(tr("Reload the current page"));
    }
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_UpdateStatusbar(const QString &string)
{
    statusBar()->showMessage(string, 2000);
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_UpdateWindowTitle(const QString &title)
{
    if (title.isEmpty())
    {
        setWindowTitle(tr("Qt Demo Browser"));
    }
    else
    {
        setWindowTitle(tr("%1 - Qt Demo Browser", "Page title and Browser name").arg(title));
    }
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_Preferences()
{
    CFKSettingDialog *s = new CFKSettingDialog(this);
    s->show();
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_FileNew()
{
    CFKBrowserApp::GetAppInstancePtr()->CreateMainWindow();
    CFKBrowserMainWindow *mw = CFKBrowserApp::GetAppInstancePtr()->GetMainWindow();
    mw->SLOT_Home();
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_FileOpen()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open Web Resource"), QString(),
            tr("Web Resources (*.html *.htm *.svg *.png *.gif *.svgz);;All files (*.*)"));

    if (file.isEmpty())
        return;

    SLOT_LoadPage(file);
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_FilePrintPreview()
{
    /*
    if (!GetCurrentTab())
        return;
    QPrintPreviewDialog *pDialog = new QPrintPreviewDialog(this);
    connect(pDialog, SIGNAL(paintRequested(QPrinter*)),
            GetCurrentTab(), SLOT(print(QPrinter*)));
    pDialog->exec();
    */
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_FilePrint()
{
    /*
    if (!GetCurrentTab())
        return;
    printRequested(currentTab()->page()->mainFrame());
    */
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_FilePrintToPDF()
{
    /*
    if (!GetCurrentTab() || !m_strPrinterOutputFileName.isEmpty())
        return;

    QFileInfo info(QStringLiteral("printout.pdf"));
    CFKPrintToPdfDialog *pDialog = new CFKPrintToPdfDialog(info.absoluteFilePath(), this);
    pDialog->setWindowTitle(tr("Print to PDF"));
    if (pDialog->exec() != QDialog::Accepted
            || pDialog->GetFilePath().isEmpty())
        return;

    m_strPrinterOutputFileName = pDialog->GetFilePath();
    GetCurrentTab()->page()->printToPdf(
                Invoke(this, &CFKBrowserMainWindow::__SLOT_HandlePdfPrinted),
                pDialog->GetPageLayout());
    */
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_PrivateBrowsing()
{
    if (!CFKBrowserApp::GetAppInstancePtr()->IsPrivateBrowsing())
    {
        QString title = tr("Are you sure you want to turn on private browsing?");
        QString text = tr("<b>%1</b><br><br>"
            "This action will reload all open tabs.<br>"
            "When private browsing in turned on,"
            " webpages are not added to the history,"
            " items are automatically removed from the Downloads window," \
            " new cookies are not stored, current cookies can't be accessed," \
            " site icons wont be stored, session wont be saved, " \
            " and searches are not added to the pop-up menu in the Google search box." \
            "  Until you close the window, you can still click the Back and Forward buttons" \
            " to return to the webpages you have opened.").arg(title);

        QMessageBox::StandardButton button = QMessageBox::question(this, QString(), text,
                               QMessageBox::Ok | QMessageBox::Cancel,
                               QMessageBox::Ok);

        if (button == QMessageBox::Ok)
        {
            CFKBrowserApp::GetAppInstancePtr()->SLOT_SetPrivateBorwsing(true);
        }
    }
    else
    {
        CFKBrowserApp::GetAppInstancePtr()->SLOT_SetPrivateBorwsing(false);
    }
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_FileSaveAs()
{

}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_EditFind()
{
    if (!GetCurrentTab())
        return;

    bool bIsOk;
    QString strSearch = QInputDialog::getText(this, tr("Find"),
                                          tr("Text:"), QLineEdit::Normal,
                                          m_strLastSearch, &bIsOk);
    if (bIsOk && !strSearch.isEmpty())
    {
        m_strLastSearch = strSearch;
        GetCurrentTab()->findText(m_strLastSearch, 0,
                  Invoke(this, &CFKBrowserMainWindow::__HandleFindTextResult));
    }
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_EditFindNext()
{
    if (!GetCurrentTab() && !m_strLastSearch.isEmpty())
        return;
    GetCurrentTab()->findText(m_strLastSearch);
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_EditFindPrevious()
{
    if (!GetCurrentTab() && !m_strLastSearch.isEmpty())
        return;
    GetCurrentTab()->findText(m_strLastSearch, QWebEnginePage::FindBackward);
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_ShowBookmarksDialog()
{
    CFKBookmarksDialog* pDialog = new CFKBookmarksDialog(this);
    connect(pDialog, SIGNAL(SIGNAL_OpenUrl(QUrl)),
            m_pTabWidget, SLOT(SLOT_LoadUrlInCurrentTab(QUrl)));
    pDialog->show();
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_AddBookmark()
{
    CFKUI_WebView* pWebView = GetCurrentTab();
    QString strUrl          = pWebView->GetUrl().toString();
    QString strTitle        = pWebView->title();
    CFKAddBookmarkDialog dialog(strUrl, strTitle);
    dialog.exec();
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_ViewZoomIn()
{
    if (!GetCurrentTab())
        return;
    GetCurrentTab()->setZoomFactor(GetCurrentTab()->zoomFactor() + 0.1);
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_ViewZoomOut()
{
    if (!GetCurrentTab())
        return;
    GetCurrentTab()->setZoomFactor(GetCurrentTab()->zoomFactor() - 0.1);
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_ViewResetZoom()
{
    if (!GetCurrentTab())
        return;
    GetCurrentTab()->setZoomFactor(1.0);
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_ViewToolbar()
{
    if (m_pNavigationBar->isVisible())
    {
        __SLOT_UpdateToolbarActionText(false);
        m_pNavigationBar->close();
    }
    else
    {
        __SLOT_UpdateToolbarActionText(true);
        m_pNavigationBar->show();
    }
    m_pAutoSaver->SLOT_OnChange();
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_ViewBookmarksBar()
{
    if (m_pBookmarksToolBar->isVisible())
    {
        __SLOT_UpdateBookmarksToolbarActionText(false);
        m_pBookmarksToolBar->close();
    }
    else
    {
        __SLOT_UpdateBookmarksToolbarActionText(true);
        m_pBookmarksToolBar->show();
    }
    m_pAutoSaver->SLOT_OnChange();
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_ViewStatusbar()
{
    if (statusBar()->isVisible())
    {
        __UpdateStatusbarActionText(false);
        statusBar()->close();
    }
    else
    {
        __UpdateStatusbarActionText(true);
        statusBar()->show();
    }
    m_pAutoSaver->SLOT_OnChange();
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_ViewPageSource()
{
    if (!GetCurrentTab())
        return;

    QPlainTextEdit* pView = new QPlainTextEdit;
    pView->setWindowTitle(tr("Page Source of %1").arg(GetCurrentTab()->title()));
    pView->setMinimumWidth(640);
    pView->setAttribute(Qt::WA_DeleteOnClose);
    pView->show();

    GetCurrentTab()->page()->toHtml(Invoke(pView, &QPlainTextEdit::setPlainText));
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_ViewFullScreen(bool bIsEnable)
{
    if (bIsEnable)
    {
        showFullScreen();
    }
    else
    {
        if (isMinimized())
            showMinimized();
        else if (isMaximized())
            showMaximized();
        else
            showNormal();
    }
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_WebSearch()
{
    m_pToolBarSearch->GetLineEdit()->selectAll();
    m_pToolBarSearch->GetLineEdit()->setFocus();
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_ToggleInspector(bool bIsEnable)
{
    /*
    QWebEngineSettings::globalSettings()->setAttribute(QWebEngineSettings::DeveloperExtrasEnabled, bIsEnable);
    if (bIsEnable)
    {
        int nResult = QMessageBox::question(this, tr("Web Inspector"),
                                           tr("The web inspector will only work correctly for pages that were loaded after enabling.\n"
                                           "Do you want to reload all pages?"),
                                           QMessageBox::Yes | QMessageBox::No);
        if (nResult == QMessageBox::Yes)
        {
            m_pTabWidget->SLOT_ReloadAllTabs();
        }
    }
    */
    Q_UNUSED(bIsEnable);
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_AboutApplication()
{
    QMessageBox::about(this, tr("About"), tr(
        "Version %1"
        "<p>This is a browser. <p>"
        "developed by Frankie.W"
        ).arg(QCoreApplication::applicationVersion()));
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_DownloadManager()
{
    CFKBrowserApp::GetDownloadMgrPtr()->show();
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_SelectLineEdit()
{
    m_pTabWidget->GetCurrentLineEdit()->selectAll();
    m_pTabWidget->GetCurrentLineEdit()->setFocus();
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_AboutToShowBackMenu()
{
    m_pHistoryBackMenu->clear();
    if (!GetCurrentTab())
        return;

    QWebEngineHistory *pHistory = GetCurrentTab()->history();
    int nHistoryCount = pHistory->count();
    for (int i = pHistory->backItems(nHistoryCount).count() - 1; i >= 0; --i)
    {
        QWebEngineHistoryItem item = pHistory->backItems(pHistory->count()).at(i);
        QAction *pAction = new QAction(this);
        pAction->setData(-1*(nHistoryCount-i-1));
        QIcon icon = CFKBrowserApp::GetAppInstancePtr()->GetIcon(item.url());
        pAction->setIcon(icon);
        pAction->setText(item.title());
        m_pHistoryBackMenu->addAction(pAction);
    }
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_AboutToShowForwardMenu()
{
    m_pHistoryForwardMenu->clear();
    if (!GetCurrentTab())
        return;

    QWebEngineHistory *pHistory = GetCurrentTab()->history();
    int nHistoryCount = pHistory->count();
    for (int i = 0; i < pHistory->forwardItems(pHistory->count()).count(); ++i)
    {
        QWebEngineHistoryItem item = pHistory->forwardItems(nHistoryCount).at(i);
        QAction* pAction = new QAction(this);
        pAction->setData(nHistoryCount-i);
        QIcon icon = CFKBrowserApp::GetAppInstancePtr()->GetIcon(item.url());
        pAction->setIcon(icon);
        pAction->setText(item.title());
        m_pHistoryForwardMenu->addAction(pAction);
    }
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_AboutToShowWindowMenu()
{
    m_pWindowMenu->clear();
    m_pWindowMenu->addAction(m_pTabWidget->GetNextTabAction());
    m_pWindowMenu->addAction(m_pTabWidget->GetPreviousTabAction());
    m_pWindowMenu->addSeparator();
    m_pWindowMenu->addAction(tr("Downloads"),
               this, SLOT(__SLOT_DownloadManager()), QKeySequence(tr("Alt+Ctrl+L", "Download Manager")));
    m_pWindowMenu->addSeparator();

    QList<CFKBrowserMainWindow*> ListWindows = CFKBrowserApp::GetAppInstancePtr()->GetMainWindowsList();
    for (int i = 0; i < ListWindows.count(); ++i)
    {
        CFKBrowserMainWindow *pWindow = ListWindows.at(i);
        QAction *pAction = m_pWindowMenu->addAction(pWindow->windowTitle(),
                                                    this, SLOT(__SLOT_ShowWindow()));
        pAction->setData(i);
        pAction->setCheckable(true);
        if (pWindow == this)
        {
            pAction->setChecked(true);
        }
    }
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_OpenActionUrl(QAction* pAction)
{
    int nOffset = pAction->data().toInt();
    QWebEngineHistory *pHistory = GetCurrentTab()->history();
    if (nOffset < 0)
    {
        pHistory->goToItem(pHistory->backItems(-1*nOffset).first()); // back
    }
    else if (nOffset > 0)
    {
        pHistory->goToItem(pHistory->forwardItems(pHistory->count() - nOffset + 1).back()); // forward
    }
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_ShowWindow()
{
    if (QAction *pAction = qobject_cast<QAction*>(sender()))
    {
        QVariant v = pAction->data();
        if (v.canConvert<int>())
        {
            int nOffset = qvariant_cast<int>(v);
            QList<CFKBrowserMainWindow*> ListWindows = CFKBrowserApp::GetAppInstancePtr()->GetMainWindowsList();
            ListWindows.at(nOffset)->activateWindow();
            ListWindows.at(nOffset)->GetCurrentTab()->setFocus();
        }
    }
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_SwapFocus()
{
    if (GetCurrentTab()->hasFocus())
    {
        m_pTabWidget->GetCurrentLineEdit()->setFocus();
    }
    else
    {
        GetCurrentTab()->setFocus();
    }
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_HandlePdfPrinted(const QByteArray& result)
{
    if (!result.size())
        return;

    QFile file(m_strPrinterOutputFileName);

    m_strPrinterOutputFileName.clear();
    if (!file.open(QFile::WriteOnly))
        return;

    file.write(result.data(), result.size());
    file.close();
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_GeometryChangeRequested(const QRect &geometry)
{
    setGeometry(geometry);
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_UpdateToolbarActionText(bool bIsVisible)
{
    m_pActionViewToolbar->setText(!bIsVisible ?
                           tr("Show Toolbar") : tr("Hide Toolbar"));
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SLOT_UpdateBookmarksToolbarActionText(bool bIsVisible)
{
    m_pActionViewBookmarkBar->setText(!bIsVisible ?
                           tr("Show Bookmarks bar") : tr("Hide Bookmarks bar"));
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__LoadDefaultState()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("BrowserMainWindow"));
    QByteArray strData = settings.value(QLatin1String("defaultState")).toByteArray();
    restoreState(strData);
    settings.endGroup();
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SetupMenu()
{
    new QShortcut(QKeySequence(Qt::Key_F6),
                  this, SLOT(__SLOT_SwapFocus()));

    // File
    QMenu* pFileMenu = menuBar()->addMenu(tr("&File"));

    pFileMenu->addAction(tr("&New Window"),
                         this, SLOT(__SLOT_FileNew()), QKeySequence::New);
    pFileMenu->addAction(m_pTabWidget->GetNewTabAction());
    pFileMenu->addAction(tr("&Open File..."),
                         this, SLOT(__SLOT_FileOpen()), QKeySequence::Open);
    pFileMenu->addAction(tr("Open &Location..."),
                         this, SLOT(__SLOT_SelectLineEdit()), QKeySequence(Qt::ControlModifier + Qt::Key_L));
    pFileMenu->addSeparator();
    pFileMenu->addAction(m_pTabWidget->GetCloseTabAction());
    pFileMenu->addSeparator();

    CFKBookmarksMgr *pBookmarksManager = CFKBrowserApp::GetBookmarksMgrPtr();
    pFileMenu->addAction(tr("&Import Bookmarks..."),
                         pBookmarksManager, SLOT(__SLOT_ImportBookmarks()));
    pFileMenu->addAction(tr("&Export Bookmarks..."),
                         pBookmarksManager, SLOT(__SLOT_ExportBookmarks()));
    pFileMenu->addSeparator();
    pFileMenu->addAction(tr("&Print to PDF..."),
                         this, SLOT(__SLOT_FilePrintToPDF()));
    pFileMenu->addSeparator();

    QAction *pAction = pFileMenu->addAction(tr("Private &Browsing..."), this, SLOT(slotPrivateBrowsing()));
    pAction->setCheckable(true);
    pAction->setChecked(CFKBrowserApp::GetAppInstancePtr()->IsPrivateBrowsing());
    connect(CFKBrowserApp::GetAppInstancePtr(), SIGNAL(SIGNAL_PrivateBrowsingChanged(bool)),
            pAction, SLOT(setChecked(bool)));
    pFileMenu->addSeparator();


    pFileMenu->addAction(tr("&Quit"),
                  this, SLOT(close()), QKeySequence(Qt::CTRL | Qt::Key_Q));


    // Edit
    QMenu* pEditMenu        = menuBar()->addMenu(tr("&Edit"));

    QAction* pUndoAction    = pEditMenu->addAction(tr("&Undo"));
    pUndoAction->setShortcuts(QKeySequence::Undo);
    m_pTabWidget->AddWebAction(pUndoAction, QWebEnginePage::Undo);
    QAction* pRedoAction    = pEditMenu->addAction(tr("&Redo"));
    pRedoAction->setShortcuts(QKeySequence::Redo);
    m_pTabWidget->AddWebAction(pRedoAction, QWebEnginePage::Redo);
    pEditMenu->addSeparator();

    QAction* pCutAction     = pEditMenu->addAction(tr("Cu&t"));
    pCutAction->setShortcuts(QKeySequence::Cut);
    m_pTabWidget->AddWebAction(pCutAction, QWebEnginePage::Cut);
    QAction* pCopyAction    = pEditMenu->addAction(tr("&Copy"));
    pCopyAction->setShortcuts(QKeySequence::Copy);
    m_pTabWidget->AddWebAction(pCopyAction, QWebEnginePage::Copy);
    QAction* pPasteAction   = pEditMenu->addAction(tr("&Paste"));
    pPasteAction->setShortcuts(QKeySequence::Paste);
    m_pTabWidget->AddWebAction(pPasteAction, QWebEnginePage::Paste);
    pEditMenu->addSeparator();

    QAction *pFindAction    = pEditMenu->addAction(tr("&Find"));
    pFindAction->setShortcuts(QKeySequence::Find);
    connect(pFindAction, SIGNAL(triggered()),
                this, SLOT(__SLOT_EditFind()));
    new QShortcut(QKeySequence(Qt::Key_Slash),
                this, SLOT(__SLOT_EditFind()));

    QAction *pFindNext      = pEditMenu->addAction(tr("&Find Next"));
    pFindNext->setShortcuts(QKeySequence::FindNext);
    connect(pFindNext, SIGNAL(triggered()),
                this, SLOT(__SLOT_EditFindNext()));

    QAction *pFindPrevious  = pEditMenu->addAction(tr("&Find Previous"));
    pFindPrevious->setShortcuts(QKeySequence::FindPrevious);
    connect(pFindPrevious, SIGNAL(triggered()),
                this, SLOT(__SLOT_EditFindPrevious()));
    pEditMenu->addSeparator();

    pEditMenu->addAction(tr("&Preferences"),
                this, SLOT(__SLOT_Preferences()), tr("Ctrl+,"));

    // View
    QMenu* pViewMenu = menuBar()->addMenu(tr("&View"));

    m_pActionViewBookmarkBar = new QAction(this);
    __SLOT_UpdateBookmarksToolbarActionText(true);
    m_pActionViewBookmarkBar->setShortcut(tr("Shift+Ctrl+B"));
    connect(m_pActionViewBookmarkBar, SIGNAL(triggered()),
            this, SLOT(__SLOT_ViewBookmarksBar()));
    pViewMenu->addAction(m_pActionViewBookmarkBar);

    m_pActionViewToolbar = new QAction(this);
    __SLOT_UpdateToolbarActionText(true);
    m_pActionViewToolbar->setShortcut(tr("Ctrl+|"));
    connect(m_pActionViewToolbar, SIGNAL(triggered()),
            this, SLOT(__SLOT_ViewToolbar()));
    pViewMenu->addAction(m_pActionViewToolbar);

    m_pActionViewStatusbar = new QAction(this);
    __UpdateStatusbarActionText(true);
    m_pActionViewStatusbar->setShortcut(tr("Ctrl+/"));
    connect(m_pActionViewStatusbar, SIGNAL(triggered()),
            this, SLOT(__SLOT_ViewStatusbar()));
    pViewMenu->addAction(m_pActionViewStatusbar);
    pViewMenu->addSeparator();

    m_pActionStop = pViewMenu->addAction(tr("&Stop"));
    QList<QKeySequence> ListShortcuts;
    ListShortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Period));
    ListShortcuts.append(Qt::Key_Escape);
    m_pActionStop->setShortcuts(ListShortcuts);
    m_pTabWidget->AddWebAction(m_pActionStop, QWebEnginePage::Stop);

    m_pActionReload = pViewMenu->addAction(tr("Reload Page"));
    m_pActionReload->setShortcuts(QKeySequence::Refresh);
    m_pTabWidget->AddWebAction(m_pActionReload, QWebEnginePage::Reload);

    pViewMenu->addAction(tr("Zoom &In"), this,
                         SLOT(__SLOT_ViewZoomIn()), QKeySequence(Qt::CTRL | Qt::Key_Plus));
    pViewMenu->addAction(tr("Zoom &Out"), this,
                         SLOT(__SLOT_ViewZoomOut()), QKeySequence(Qt::CTRL | Qt::Key_Minus));
    pViewMenu->addAction(tr("Reset &Zoom"), this,
                         SLOT(__SLOT_ViewResetZoom()), QKeySequence(Qt::CTRL | Qt::Key_0));

    pViewMenu->addSeparator();
    pViewMenu->addAction(tr("Page S&ource"), this,
                         SLOT(__SLOT_ViewPageSource()), tr("Ctrl+Alt+U"));
    QAction *pFullscreenAction = pViewMenu->addAction(tr("&Full Screen"),
                   this, SLOT(__SLOT_ViewFullScreen(bool)),  Qt::Key_F11);
    pFullscreenAction->setCheckable(true);

    // History
    CFKHistoryMenu *pHistoryMenu = new CFKHistoryMenu(this);
    connect(pHistoryMenu, SIGNAL(SIGNAL_OpenUrl(QUrl)),
            m_pTabWidget, SLOT(SLOT_LoadUrlInCurrentTab(QUrl)));
    connect(pHistoryMenu, SIGNAL(SIGNAL_Hovered(QString)),
            this, SLOT(__SLOT_UpdateStatusbar(QString)));
    pHistoryMenu->setTitle(tr("Hi&story"));
    menuBar()->addMenu(pHistoryMenu);

    QList<QAction*> ListHistoryActions;

    m_pActionHistoryBack = new QAction(tr("Back"), this);
    m_pTabWidget->AddWebAction(m_pActionHistoryBack, QWebEnginePage::Back);
    m_pActionHistoryBack->setShortcuts(QKeySequence::Back);
    m_pActionHistoryBack->setIconVisibleInMenu(false);
    ListHistoryActions.append(m_pActionHistoryBack);

    m_pActionHistoryForward = new QAction(tr("Forward"), this);
    m_pTabWidget->AddWebAction(m_pActionHistoryForward, QWebEnginePage::Forward);
    m_pActionHistoryForward->setShortcuts(QKeySequence::Forward);
    m_pActionHistoryForward->setIconVisibleInMenu(false);
    ListHistoryActions.append(m_pActionHistoryForward);

    QAction* pActionHistoryHome = new QAction(tr("Home"), this);
    connect(pActionHistoryHome, SIGNAL(triggered()),
            this, SLOT(SLOT_Home()));
    pActionHistoryHome->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_H));
    ListHistoryActions.append(pActionHistoryHome);

    pHistoryMenu->SetInitActions(ListHistoryActions);

    // Bookmarks
    CFKBookmarksMenu* pBookmarksMenu = new CFKBookmarksMenu(this);
    connect(pBookmarksMenu, SIGNAL(SIGNAL_OpenUrl(QUrl)),
            m_pTabWidget, SLOT(SLOT_LoadUrlInCurrentTab(QUrl)));
    connect(pBookmarksMenu, SIGNAL(SIGNAL_Hovered(QString)),
            this, SLOT(__SLOT_UpdateStatusbar(QString)));
    pBookmarksMenu->setTitle(tr("&Bookmarks"));
    menuBar()->addMenu(pBookmarksMenu);

    QList<QAction*> ListBookmarksActions;

    QAction* pShowAllBookmarksAction = new QAction(tr("Show All Bookmarks"), this);
    connect(pShowAllBookmarksAction, SIGNAL(triggered()),
            this, SLOT(__SLOT_ShowBookmarksDialog()));
    m_pActionAddBookmark = new QAction(QIcon(QLatin1String(":addbookmark.png")), tr("Add Bookmark..."), this);
    m_pActionAddBookmark->setIconVisibleInMenu(false);

    connect(m_pActionAddBookmark, SIGNAL(triggered()),
            this, SLOT(__SLOT_AddBookmark()));
    m_pActionAddBookmark->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));

    ListBookmarksActions.append(pShowAllBookmarksAction);
    ListBookmarksActions.append(m_pActionAddBookmark);
    pBookmarksMenu->SetInitialActions(ListBookmarksActions);

    // Window
    m_pWindowMenu = menuBar()->addMenu(tr("&Window"));
    connect(m_pWindowMenu, SIGNAL(aboutToShow()),
            this, SLOT(__SLOT_AboutToShowWindowMenu()));
    __SLOT_AboutToShowWindowMenu();

    QMenu *pToolsMenu = menuBar()->addMenu(tr("&Tools"));
    pToolsMenu->addAction(tr("Web &Search"),
                          this, SLOT(__SLOT_WebSearch()), QKeySequence(tr("Ctrl+K", "Web Search")));

    QMenu *pHelpMenu = menuBar()->addMenu(tr("&Help"));
    pHelpMenu->addAction(tr("About &Qt"),
                         qApp, SLOT(aboutQt()));
    pHelpMenu->addAction(tr("About &Demo Browser"),
                         this, SLOT(__SLOT_AboutApplication()));
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__SetupToolBar()
{
    m_pNavigationBar = addToolBar(tr("Navigation"));
    connect(m_pNavigationBar->toggleViewAction(), SIGNAL(toggled(bool)),
            this, SLOT(__SLOT_UpdateToolbarActionText(bool)));

    m_pActionHistoryBack->setIcon(style()->standardIcon(QStyle::SP_ArrowBack, 0, this));
    m_pHistoryBackMenu = new QMenu(this);
    m_pActionHistoryBack->setMenu(m_pHistoryBackMenu);
    connect(m_pHistoryBackMenu, SIGNAL(aboutToShow()),
            this, SLOT(__SLOT_AboutToShowBackMenu()));
    connect(m_pHistoryBackMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(__SLOT_OpenActionUrl(QAction*)));
    m_pNavigationBar->addAction(m_pActionHistoryBack);

    m_pActionHistoryForward->setIcon(style()->standardIcon(QStyle::SP_ArrowForward, 0, this));
    m_pHistoryForwardMenu = new QMenu(this);
    connect(m_pHistoryForwardMenu, SIGNAL(aboutToShow()),
            this, SLOT(__SLOT_AboutToShowForwardMenu()));
    connect(m_pHistoryForwardMenu, SIGNAL(triggered(QAction*)),
            this, SLOT(__SLOT_OpenActionUrl(QAction*)));
    m_pActionHistoryForward->setMenu(m_pHistoryForwardMenu);
    m_pNavigationBar->addAction(m_pActionHistoryForward);

    m_pActionStopReload = new QAction(this);
    m_ReloadIcon = style()->standardIcon(QStyle::SP_BrowserReload);
    m_pActionStopReload->setIcon(m_ReloadIcon);

    m_pNavigationBar->addAction(m_pActionStopReload);

    m_pNavigationBar->addWidget(m_pTabWidget->GetLineEditStack());

    m_pToolBarSearch = new CFKUI_ToolBarSearch(m_pNavigationBar);
    m_pNavigationBar->addWidget(m_pToolBarSearch);
    connect(m_pToolBarSearch, SIGNAL(SIGNAL_Search(QUrl)),
            SLOT(__SLOT_LoadUrl(QUrl)));

    m_pChaseWidget = new CFKUI_ChaseWidget(this);
    m_pNavigationBar->addWidget(m_pChaseWidget);
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__UpdateStatusbarActionText(bool bIsVisible)
{
    m_pActionViewStatusbar->setText(!bIsVisible ?
          tr("Show Status Bar") : tr("Hide Status Bar"));
}
//-----------------------------------------------------
void CFKBrowserMainWindow::__HandleFindTextResult(bool bIsFound)
{
    if (!bIsFound)
        __SLOT_UpdateStatusbar(tr("\"%1\" not found.").arg(m_strLastSearch));
}
//-----------------------------------------------------
