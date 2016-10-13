/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKBROWSERAPP_H
#define FKBROWSERAPP_H
//-----------------------------------------------------
#include <QtWidgets/QApplication>

#include <QtCore/QPointer>
#include <QtCore/QUrl>

#include <QtGui/QIcon>
//-----------------------------------------------------
QT_BEGIN_NAMESPACE
class QLocalServer;
class QWebEngineProfile;
class QNetworkAccessManager;
QT_END_NAMESPACE
//-----------------------------------------------------
class CFKBrowserMainWindow;     // 浏览器单选项卡界面
class CFKDownloadDialog;        // 下载管理器
class CFKHistoryMgr;            // 浏览历史管理器
class CFKBookmarksMgr;          // 收藏夹管理器
class CFKCookieJarMgr;          // Cookie信息管理器
//-----------------------------------------------------
class CFKBrowserApp : public QApplication
{
    // 有槽对象，必须添加本宏定义
    Q_OBJECT
signals:
    void                            SIGNAL_PrivateBrowsingChanged(bool);
public:
    // 默认构造函数
    CFKBrowserApp(int& argc, char** argv);
    ~CFKBrowserApp();
public:
    // 对外单例接口
    static CFKBrowserApp*           GetAppInstancePtr();
    static CFKDownloadDialog*       GetDownloadMgrPtr();
    static CFKHistoryMgr*           GetHistoryMgrPtr();
    static CFKBookmarksMgr*         GetBookmarksMgrPtr();
    static QNetworkAccessManager*   GetNetworkAccessMgrPtr();
    static CFKCookieJarMgr*         GetCookieJarMgrPtr();
public:
    bool                            IsTheOnlyBrowser() const;
public:
    // 获取当前浏览器第一页面窗口
    CFKBrowserMainWindow*           GetMainWindow();
    // 获取浏览器页面窗口列表
    QList<CFKBrowserMainWindow*>    GetMainWindowsList();
    // 清除全部无效浏览器页面窗口
    void                            CleanAllEmptyMainWindows();
    // 创建一个浏览器页面
    CFKBrowserMainWindow*           CreateMainWindow();
    // 获取ICON对象
    QIcon                           GetIcon(const QUrl& url)const;
    QIcon                           GetDefaultIcon() const;
    // 获取当前Homepage的Url
    QString                         GetHomepageUrl() const;
    bool                            IsPrivateBrowsing() const;

    void                            SaveSession();

    // 加载配置文件
    void                            LoadSettingConfig();
public slots:
    // 创建一个本地socket服务端连接
    void                            SLOT_CreateLocalServerConnection();
    // 第二步初始化
    void                            SLOT_PostLaunch();
    void                            SLOT_SetPrivateBorwsing(bool bIsPrivateBrowsing);
private:
    // 获取命令行参数字符串
    QString                         __GetCommandLineUrlArgument() const;
    // 测试服务器连接
    bool                            __IsCanConnectSvr(QString strServerName, int nTimeOut = 500);
    // 初始化本地服务器
    bool                            __InitLocalServer(QString strServerName);
    // 初始化多国语言支持
    void                            __InitTranslator();
    // 初始化软件基本信息
    void                            __InitAppInfos();
    // 打开指定Url
    void                            __OpenUrl(const QUrl& url);
    // 设置用户CSS样式
    void                            __SetUserStyleSheet(QWebEngineProfile* pProfile,
                                        const QString& strStyleSheet, CFKBrowserMainWindow* pMainWindow = 0);
private:
    QLocalServer*                           m_pLocalServer;         // 本地socket服务端
    QList<QPointer<CFKBrowserMainWindow>>   m_ListMainWindows;      // 浏览器主窗口列表
    QWebEngineProfile*                      m_pWebEngineProfile;    // 多页面共享的浏览器配置文件
    bool                                    m_bIsPrivateBrowsing;   // 是否是私有浏览
    QByteArray                              m_strLastSession;       // 上一次打开的连接(被记录在session中)
    QByteArray                              m_strHomepageUrl;       // 默认主页Url
    int                                     m_nNextLinkTabIndex;    // 配置中上一次打开的选项卡编号
    mutable QIcon                           m_pDefaultIcon;         // 浏览器默认ICON
private:
    static CFKDownloadDialog*               s_pDownloadMgr;
    static CFKHistoryMgr*                   s_pHistoryMgr;
    static CFKBookmarksMgr*                 s_pBookmarksMgr;
    static QNetworkAccessManager*           s_pNetworkAccessMgr;
};
//-----------------------------------------------------
#endif // FKBROWSERAPP_H
