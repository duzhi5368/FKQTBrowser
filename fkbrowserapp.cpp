/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QLibraryInfo>
#include <QtCore/QSettings>
#include <QtCore/QTextStream>
#include <QtCore/QTranslator>
#include <QtCore/QTimer>
#include <QtCore/QBuffer>

#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkAccessManager>

#include <QtWidgets/QMessageBox>

#include <QtGui/QDesktopServices>
#include <QtGui/QFont>
#include <QtGui/QIcon>

#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
//-----------------------------------------------------
#include "fkbrowserapp.h"
#include "fkbrowsermainwindow.h"
#include "fkcookiejarmgr.h"
#include "fkdownloaddialog.h"
#include "fkhistorymgr.h"
#include "fkbookmarksmgr.h"
#include "fkui_tabwidget.h"
//-----------------------------------------------------
CFKDownloadDialog*      CFKBrowserApp::s_pDownloadMgr       = NULL;
CFKHistoryMgr*          CFKBrowserApp::s_pHistoryMgr        = NULL;
CFKBookmarksMgr*        CFKBrowserApp::s_pBookmarksMgr      = NULL;
QNetworkAccessManager*  CFKBrowserApp::s_pNetworkAccessMgr  = NULL;
//-----------------------------------------------------
CFKBrowserApp::CFKBrowserApp(int& argc, char** argv)
    : QApplication(argc, argv)
    , m_pLocalServer( NULL )
    , m_pWebEngineProfile( NULL )
    , m_bIsPrivateBrowsing( false )
    , m_nNextLinkTabIndex( 1 )
{
    // 变量初始化
    m_ListMainWindows.clear();
    m_strLastSession.clear();
    m_strHomepageUrl.clear();

    // 初始化软件基本信息
    __InitAppInfos();

    // 当前服务器名 FKWebBrowser_570_001
    QString strServerName = QCoreApplication::applicationName()
        + QLatin1String("_")
        + QString::fromLatin1(QT_VERSION_STR).remove('.')
        + QLatin1String("_")
        + QCoreApplication::applicationVersion().remove('.');

    // 创建本地Socket测试
    if( !__IsCanConnectSvr(strServerName) )
    {
        return;
    }

    // 初始化本地服务器
    if( !__InitLocalServer(strServerName))
    {
        return;
    }

    // 打开页面
    QDesktopServices::setUrlHandler(QLatin1String("http"), this, "openUrl");

    // 初始化多国语言支持
    __InitTranslator();

    // 程序设置，关闭最后一个窗口时清除进程
    QApplication::setQuitOnLastWindowClosed(true);

    // 延迟10毫秒，进行第二部初始化
    QTimer::singleShot(0, this, SLOT(SLOT_PostLaunch()));
}
//-----------------------------------------------------
CFKBrowserApp::~CFKBrowserApp()
{
}
//-----------------------------------------------------
CFKBrowserApp* CFKBrowserApp::GetAppInstancePtr()
{
    return (dynamic_cast<CFKBrowserApp *>(QCoreApplication::instance()));
}
//-----------------------------------------------------
CFKDownloadDialog* CFKBrowserApp::GetDownloadMgrPtr()
{
    if(s_pDownloadMgr == NULL)
        s_pDownloadMgr = new CFKDownloadDialog();\
    return s_pDownloadMgr;
}
//-----------------------------------------------------
CFKHistoryMgr* CFKBrowserApp::GetHistoryMgrPtr()
{
    if(s_pHistoryMgr == NULL)
        s_pHistoryMgr = new CFKHistoryMgr();
    return s_pHistoryMgr;
}
//-----------------------------------------------------
CFKBookmarksMgr* CFKBrowserApp::GetBookmarksMgrPtr()
{
    if(s_pBookmarksMgr == NULL)
        s_pBookmarksMgr = new CFKBookmarksMgr();
    return s_pBookmarksMgr;
}
//-----------------------------------------------------
QNetworkAccessManager* CFKBrowserApp::GetNetworkAccessMgrPtr()
{
    if(s_pNetworkAccessMgr == NULL)
        s_pNetworkAccessMgr = new QNetworkAccessManager();
    return s_pNetworkAccessMgr;
}
//-----------------------------------------------------
CFKCookieJarMgr* CFKBrowserApp::GetCookieJarMgrPtr()
{
    return dynamic_cast<CFKCookieJarMgr*>(GetNetworkAccessMgrPtr()->cookieJar());
}
//-----------------------------------------------------
bool CFKBrowserApp::IsTheOnlyBrowser() const
{
    return (m_pLocalServer != NULL);
}
//-----------------------------------------------------
// 获取当前浏览器第一页面窗口
CFKBrowserMainWindow* CFKBrowserApp::GetMainWindow()
{
    CleanAllEmptyMainWindows();

    // 若没有有效窗口，创建
    if(m_ListMainWindows.isEmpty())
    {
        CreateMainWindow();
    }
    return m_ListMainWindows[0];
}
//-----------------------------------------------------
// 获取浏览器页面窗口列表
QList<CFKBrowserMainWindow*> CFKBrowserApp::GetMainWindowsList()
{
    CleanAllEmptyMainWindows();
    // 去空窗口
    QList<CFKBrowserMainWindow*>    MainWindowsList;
    for(int i = 0; i < m_ListMainWindows.count(); ++i)
    {
        MainWindowsList.append(m_ListMainWindows.at(i));
    }
    return MainWindowsList;
}
//-----------------------------------------------------
// 清除全部浏览器页面窗口
void CFKBrowserApp::CleanAllEmptyMainWindows()
{
    for(int i = m_ListMainWindows.count() - 1; i >= 0; --i)
    {
        if(m_ListMainWindows.at(i).isNull())
        {
            m_ListMainWindows.removeAt(i);
        }
    }
}
//-----------------------------------------------------
// 创建一个浏览器页面 SLOT
CFKBrowserMainWindow* CFKBrowserApp::CreateMainWindow()
{
    CFKBrowserMainWindow* pWindow = new CFKBrowserMainWindow();
    m_ListMainWindows.prepend(pWindow);
    pWindow->show();
    return pWindow;
}
//-----------------------------------------------------
// 获取ICON对象
QIcon CFKBrowserApp::GetIcon(const QUrl& url)const
{
    /*
    QIcon tagIcon = QWebEngineSettings::iconForUrl(url);
    if(!tagIcon.isNull())
    {
        return tagIcon.pixmap(16,16);
    }
    */
    Q_UNUSED(url);
    return GetDefaultIcon();
}
//-----------------------------------------------------
// 获取默认ICON对象
QIcon CFKBrowserApp::GetDefaultIcon() const
{
    if (m_pDefaultIcon.isNull())
        m_pDefaultIcon = (QIcon(QLatin1String(":defaulticon.png")));
    return m_pDefaultIcon;
}
//-----------------------------------------------------
// 获取当前Homepage的Url
QString CFKBrowserApp::GetHomepageUrl() const
{
    return m_strHomepageUrl;
}
//-----------------------------------------------------
bool CFKBrowserApp::IsPrivateBrowsing() const
{
    return m_bIsPrivateBrowsing;
}
//-----------------------------------------------------
void CFKBrowserApp::SaveSession()
{
    if (m_bIsPrivateBrowsing)
        return;

    CleanAllEmptyMainWindows();

    QSettings settings;
    settings.beginGroup(QLatin1String("sessions"));

    QByteArray  strData;
    QBuffer     buffer(&strData);
    QDataStream stream(&buffer);
    buffer.open(QIODevice::ReadWrite);

    stream << m_ListMainWindows.count();
    for (int i = 0; i < m_ListMainWindows.count(); ++i)
    {
        stream << m_ListMainWindows.at(i)->saveState();
    }
    settings.setValue(QLatin1String("lastSession"), strData);
    settings.endGroup();
}
//-----------------------------------------------------
// 创建一个本地socket服务端连接 SLOT
void CFKBrowserApp::SLOT_CreateLocalServerConnection()
{
    QLocalSocket* pSocket = m_pLocalServer->nextPendingConnection();
    if( pSocket == NULL )
    {
        return;
    }

    // 连接等待1秒
    pSocket->waitForReadyRead(1000);

    QTextStream strStream(pSocket);
    QString strUrl;
    strStream >> strUrl;

    // 获取Url失败
    if(strUrl.isEmpty())
    {
        delete pSocket;
        return;
    }

    // 创建Tab
    if (m_nNextLinkTabIndex == 1)
        CreateMainWindow();
    else
        GetMainWindow()->GetTabWidget()->SLOT_NewTab();

    // 打开指定Url
    __OpenUrl(strUrl);

    // 置顶并激活当前窗口
    GetMainWindow()->raise();
    GetMainWindow()->activateWindow();

    // 释放socket
    delete pSocket;
}
//-----------------------------------------------------
// 第二步初始化
void CFKBrowserApp::SLOT_PostLaunch()
{
    // 设置网络文件保存路径
    QString strDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    if(strDir.isEmpty())
    {
        strDir = QDir::homePath() + QLatin1String("/.") + QCoreApplication::applicationName();
    }

    // 加载配置文件
    LoadSettingConfig();

    // 打开起始页面
    if(m_ListMainWindows.count() > 0)
    {
        const QString strUrl = __GetCommandLineUrlArgument();
        if(strUrl.isEmpty())
        {
            // 若没有起始页面，则打开Homepage
            GetMainWindow()->SLOT_Home();
        }
        else
        {
            GetMainWindow()->SLOT_LoadPage(strUrl);
        }
    }

    // 初始化历史管理器
    CFKBrowserApp::GetHistoryMgrPtr();
}
//-----------------------------------------------------
void CFKBrowserApp::SLOT_SetPrivateBorwsing(bool bIsPrivateBrowsing)
{
    if (m_bIsPrivateBrowsing == bIsPrivateBrowsing)
        return;
    m_bIsPrivateBrowsing = bIsPrivateBrowsing;

    if (bIsPrivateBrowsing)
    {
        if (!m_pWebEngineProfile)
            m_pWebEngineProfile = new QWebEngineProfile(this);
        Q_FOREACH (CFKBrowserMainWindow* window, GetMainWindowsList())
        {
            window->GetTabWidget()->SetProfile(m_pWebEngineProfile);
        }
    }
    else
    {
        Q_FOREACH (CFKBrowserMainWindow* window, GetMainWindowsList())
        {
            window->GetTabWidget()->SetProfile(QWebEngineProfile::defaultProfile());
            window->m_strLastSearch = QString::null;
            window->GetTabWidget()->clear();
        }
    }
    emit SIGNAL_PrivateBrowsingChanged(bIsPrivateBrowsing);
}
//-----------------------------------------------------
// 获取命令行参数字符串
QString CFKBrowserApp::__GetCommandLineUrlArgument() const
{
    const QStringList args = QCoreApplication::arguments();
    if (args.count() > 1)
    {
        const QString strLastArg = args.last();
        const bool bIsValidUrl = QUrl::fromUserInput(strLastArg).isValid();
        if (bIsValidUrl)
        {
            return strLastArg;
        }
    }

     return QString();
}
//-----------------------------------------------------
// 测试服务器连接
bool CFKBrowserApp::__IsCanConnectSvr(QString strServerName, int nTimeOut)
{
    QLocalSocket localSocket;
    localSocket.connectToServer(strServerName);
    if(localSocket.waitForConnected(nTimeOut))
    {
        QTextStream stream(&localSocket);
        stream << __GetCommandLineUrlArgument();
        stream.flush();
        localSocket.waitForBytesWritten();
#ifdef QT_DEBUG
        QMessageBox::information(NULL, "Error",
                  tr("Connect server %1 failed!").arg(strServerName));
#endif
        return false;
    }
    return true;
}
//-----------------------------------------------------
// 初始化本地服务器
bool CFKBrowserApp::__InitLocalServer(QString strServerName)
{
    m_pLocalServer = new QLocalServer(this);
    // 绑定本地插槽回调函数
    connect(m_pLocalServer, SIGNAL(newConnection()), this, SLOT(SLOT_CreateLocalServerConnection()));
    if( !m_pLocalServer->listen(strServerName) &&
        m_pLocalServer->serverError() == QAbstractSocket::AddressInUseError)
    {
        QLocalServer::removeServer(strServerName);
        if (!m_pLocalServer->listen(strServerName))
        {
            qWarning("Could not create local socket %s.", qPrintable(strServerName));
        }
#ifdef QT_DEBUG
        QMessageBox::information(NULL, "Error",
                  tr("Create server %1 failed!").arg(strServerName));
#endif
        return false;
    }
    return true;
}
//-----------------------------------------------------
// 初始化多国语言支持
void CFKBrowserApp::__InitTranslator()
{
    QString strLocalSysName = QLocale::system().name();
    QTranslator* pTranslator = new QTranslator(this);
    pTranslator->load(strLocalSysName, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    QApplication::installTranslator(pTranslator);
}
//-----------------------------------------------------
// 初始化软件基本信息
void CFKBrowserApp::__InitAppInfos()
{
    // 组织名
    QCoreApplication::setOrganizationName(QLatin1String("Intevalue"));
    // 应用程序名
    QCoreApplication::setApplicationName(QLatin1String("FKWebBrowser"));
    // 当前版本号
    QCoreApplication::setApplicationVersion(QLatin1String("0.0.1"));
    // 设置窗口Icon
    setWindowIcon(QIcon(QLatin1String(":ICON.svg")));
}
//-----------------------------------------------------
// 加载配置文件
void CFKBrowserApp::LoadSettingConfig()
{
    // 加载配置文件
    QSettings           tagSettings;
    QWebEngineSettings* pDefaultSettings    = QWebEngineSettings::globalSettings();
    QWebEngineProfile*  pDefaultProfile     = QWebEngineProfile::defaultProfile();

    {   // 上次退出最后打开的连接Url
        tagSettings.beginGroup(QLatin1String("sessions"));
        m_strLastSession = tagSettings.value(QLatin1String("lastSession")).toByteArray();
        tagSettings.endGroup();
    }

    {   // 默认主页连接Url
        tagSettings.beginGroup(QLatin1String("usersetting"));
        m_strHomepageUrl = tagSettings.value(QLatin1String("homepage")).toByteArray();
        tagSettings.endGroup();
    }

    {   // 当前打开的连接页面数
        tagSettings.beginGroup(QLatin1String("general"));
        m_nNextLinkTabIndex = tagSettings.value(QLatin1String("openLinksIn"), 0).toInt();
        tagSettings.endGroup();
    }

    {
        tagSettings.beginGroup(QLatin1String("websettings"));

        // 修改字体
        QString strStandardFontFamily           = pDefaultSettings->fontFamily(QWebEngineSettings::StandardFont);
        int nStandardFontSize                   = pDefaultSettings->fontSize(QWebEngineSettings::DefaultFontSize);
        QFont tagStandardFont                   = QFont(strStandardFontFamily, nStandardFontSize);
        tagStandardFont                         = qvariant_cast<QFont>(tagSettings.value(QLatin1String("standardFont"), tagStandardFont));
        pDefaultSettings->setFontFamily(QWebEngineSettings::StandardFont, tagStandardFont.family());
        pDefaultSettings->setFontSize(QWebEngineSettings::DefaultFontSize, tagStandardFont.pointSize());

        // 修改字体
        QString strFixedFontFamily              = pDefaultSettings->fontFamily(QWebEngineSettings::FixedFont);
        int nFixedFontSize                      = pDefaultSettings->fontSize(QWebEngineSettings::DefaultFixedFontSize);
        QFont tagFixedFont                      = QFont(strFixedFontFamily, nFixedFontSize);
        tagFixedFont                            = qvariant_cast<QFont>(tagSettings.value(QLatin1String("fixedFont"), tagFixedFont));
        pDefaultSettings->setFontFamily(QWebEngineSettings::FixedFont, tagFixedFont.family());
        pDefaultSettings->setFontSize(QWebEngineSettings::DefaultFixedFontSize, tagFixedFont.pointSize());

        // 修改一些列用户习惯参数
        pDefaultSettings->setAttribute(QWebEngineSettings::JavascriptEnabled, tagSettings.value(QLatin1String("enableJavascript"), true).toBool());
        pDefaultSettings->setAttribute(QWebEngineSettings::ScrollAnimatorEnabled, tagSettings.value(QLatin1String("enableScrollAnimator"), true).toBool());
        pDefaultSettings->setAttribute(QWebEngineSettings::PluginsEnabled, tagSettings.value(QLatin1String("enablePlugins"), true).toBool());
        pDefaultSettings->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, tagSettings.value(QLatin1String("enableFullSupport"), true).toBool());
        pDefaultProfile->setHttpUserAgent(tagSettings.value(QLatin1String("httpUserAgent")).toString());
        pDefaultProfile->setHttpAcceptLanguage(tagSettings.value(QLatin1String("httpAcceptLanguage")).toString());

        // 插入脚本，修改页面为用户自定义CSS样式
        QString strCSSStyle = tagSettings.value(QLatin1String("userStyleSheet")).toString();
        __SetUserStyleSheet(pDefaultProfile, strCSSStyle, GetMainWindow());

        // 修改用户下载方式习惯
        switch (tagSettings.value(QLatin1String("faviconDownloadMode"), 1).toInt()) {
        case 0:
            pDefaultSettings->setAttribute(QWebEngineSettings::AutoLoadIconsForPage, false);
            break;
        case 1:
            pDefaultSettings->setAttribute(QWebEngineSettings::AutoLoadIconsForPage, true);
            pDefaultSettings->setAttribute(QWebEngineSettings::TouchIconsEnabled, false);
            break;
        case 2:
            pDefaultSettings->setAttribute(QWebEngineSettings::AutoLoadIconsForPage, true);
            pDefaultSettings->setAttribute(QWebEngineSettings::TouchIconsEnabled, true);
            break;
        }
        tagSettings.endGroup();
    }

    {   // cookies策略
        tagSettings.beginGroup(QLatin1String("cookies"));
        QWebEngineProfile::PersistentCookiesPolicy persistentCookiesPolicy = QWebEngineProfile::PersistentCookiesPolicy(tagSettings.value(QLatin1String("persistentCookiesPolicy")).toInt());
        pDefaultProfile->setPersistentCookiesPolicy(persistentCookiesPolicy);
        QString pdataPath = tagSettings.value(QLatin1String("persistentDataPath")).toString();
        pDefaultProfile->setPersistentStoragePath(pdataPath);
        tagSettings.endGroup();
    }
    {   // 设置网络代理
        tagSettings.beginGroup(QLatin1String("proxy"));
        QNetworkProxy tagProxy;
        if (tagSettings.value(QLatin1String("enabled"), false).toBool()) {
            if (tagSettings.value(QLatin1String("type"), 0).toInt() == 0)
                tagProxy = QNetworkProxy::Socks5Proxy;
            else
                tagProxy = QNetworkProxy::HttpProxy;
            tagProxy.setHostName(tagSettings.value(QLatin1String("hostName")).toString());
            tagProxy.setPort(tagSettings.value(QLatin1String("port"), 1080).toInt());
            tagProxy.setUser(tagSettings.value(QLatin1String("userName")).toString());
            tagProxy.setPassword(tagSettings.value(QLatin1String("password")).toString());
        }
        QNetworkProxy::setApplicationProxy(tagProxy);
        tagSettings.endGroup();
    }
}
//-----------------------------------------------------
// 打开指定Url
void CFKBrowserApp::__OpenUrl(const QUrl& url)
{
    GetMainWindow()->SLOT_LoadPage(url.toString());
}
//-----------------------------------------------------
// 设置用户CSS样式
void CFKBrowserApp::__SetUserStyleSheet(QWebEngineProfile* pProfile,
                                    const QString& strStyleSheet, CFKBrowserMainWindow* pMainWindow)
{
    Q_ASSERT(pProfile);
    if(pProfile == NULL)
        return;

    QString strScriptName(QStringLiteral("userStyleSheet"));
    QWebEngineScript tagScript;
    QList<QWebEngineScript> styleSheets = pProfile->scripts()->findScripts(strScriptName);
    if (!styleSheets.isEmpty())
    {
        tagScript = styleSheets.first();
    }
    Q_FOREACH (const QWebEngineScript &s, styleSheets)
    {
        pProfile->scripts()->remove(s);
    }

    if (tagScript.isNull())
    {
        tagScript.setName(strScriptName);
        tagScript.setInjectionPoint(QWebEngineScript::DocumentReady);
        tagScript.setRunsOnSubFrames(true);
        tagScript.setWorldId(QWebEngineScript::ApplicationWorld);
    }
    QString strSource = QString::fromLatin1("(function() {"\
                                         "var css = document.getElementById(\"FKWebBrowser_userStyleSheet\");"\
                                         "if (css == undefined) {"\
                                         "    css = document.createElement(\"style\");"\
                                         "    css.type = \"text/css\";"\
                                         "    css.id = \"FKWebBrowser_userStyleSheet\";"\
                                         "    document.head.appendChild(css);"\
                                         "}"\
                                         "css.innerText = \"%1\";"\
                                         "})()").arg(strStyleSheet);
    tagScript.setSourceCode(strSource);
    pProfile->scripts()->insert(tagScript);
    // 在View显示之前运行该脚本
    // 将可以修改页面的css样式
    if (pMainWindow)
    {
        QMetaObject::invokeMethod(pMainWindow, "runScriptOnOpenViews",
                                  Qt::QueuedConnection, Q_ARG(QString, strSource));
    }
}
//-----------------------------------------------------
