/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKCOOKIEJARMGR_H
#define FKCOOKIEJARMGR_H
//-----------------------------------------------------
#include <QtNetwork/QNetworkCookieJar>
#include <QtCore/QUrl>
//-----------------------------------------------------
class CFKAutoSaver;         // 自动定时保存管理器
class CFKCookieModel;
//-----------------------------------------------------
class CFKCookieJarMgr : public QNetworkCookieJar
{
    Q_OBJECT

    friend class CFKCookieModel;
public:
    // cookie 接收策略
    enum ENUM_CookieAcceptPolicy{
      eCAP_AcceptAlways = 0,                // 永远接受全部Cookie
      eCAP_AcceptNever,                     // 从不接受任何Cookie
      eCAP_AcceptOnlyFromSitesNavigatedTo,  // 阻止你不信任或未导入的Cookie
    };
    // cookie 保持策略
    enum ENUM_CookieKeepPolicy{
      eCKP_KeepUntilExpire  = 0,            // 保存Cookie直到到期
      eCKP_KeepUntilExit,                   // 关闭退出后Cookie就失效
      eCKP_KeepTimeLimit,                   // 无限期保存Cookie
    };
public:
    CFKCookieJarMgr(QObject* pParent = NULL);
    ~CFKCookieJarMgr();
public:
    // 清除Cookie
    void                    ClearCookie();

    // 获取指定Url的Cookies
    QList<QNetworkCookie>   GetCookiesForUrl(const QUrl &url) const;
    // 设置指定Url的Cookies
    bool                    SetCookiesFromUrl(const QList<QNetworkCookie> &cookieList,
                                              const QUrl &url);
    // 获取Cookie接受策略
    ENUM_CookieAcceptPolicy GetAcceptPolicy() const;
    // 设置Cookie接受策略
    void                    SetAcceptPolicy(ENUM_CookieAcceptPolicy ePolicy);
    // 获取Cookie保持策略
    ENUM_CookieKeepPolicy   GetKeepPolicy() const;
    // 设置Cookie保持策略
    void                    SetKeepPolicy(ENUM_CookieKeepPolicy ePolicy);
    // 获取阻止的异常列表
    QStringList             GetBlockedCookies() const;
    // 设置阻止的异常列表
    void                    SetBlockedCookies(const QStringList& list);
    // 获取许可的异常列表
    QStringList             GetAllowedCookies() const;
    // 设置许可的异常列表
    void                    SetAllowedCookies(const QStringList& list);
    // 获取许可连接的异常列表
    QStringList             GetAllowForSessionCookies() const;
    // 设置许可连接的异常列表
    void                    SetAllowForSessionCookies(const QStringList& list);
private:
    // 加载Cookies
    void                    __LoadCookies();
    // 清除老Cookies
    void                    __PurgeOldCookies();
public slots:
    // 保存Cookies
    void                    SLOT_Save();
signals:
    // Cookies有更新消息
    void                    SIGNAL_CookiesChanged();
private:
    bool                    m_bIsLoaded;                        // 是否已读取配置
    CFKAutoSaver*           m_pAutoSaver;                       // 自动定时保存管理器
    ENUM_CookieAcceptPolicy m_eAcceptCookiePolicy;              // Cookie接受策略
    ENUM_CookieKeepPolicy   m_eKeepCookiePolicy;                // Cookie保存策略
    QStringList             m_ExceptionsBlockList;              // 阻止的异常列表
    QStringList             m_ExceptionsAllowList;              // 许可的异常列表
    QStringList             m_ExceptionsAllowForSessionList;    // 许可连接的异常列表

    /*
    Q_PROPERTY(ENUM_CookieAcceptPolicy acceptPolicy READ acceptPolicy WRITE setAcceptPolicy)
    Q_PROPERTY(ENUM_CookieKeepPolicy keepPolicy READ keepPolicy WRITE setKeepPolicy)
    Q_PROPERTY(QStringList blockedCookies READ GetBlockedCookies WRITE SetBlockedCookies)
    Q_PROPERTY(QStringList allowedCookies READ GetAllowedCookies WRITE SetAllowedCookies)
    Q_PROPERTY(QStringList allowForSessionCookies READ GetAllowForSessionCookies WRITE SetAllowForSessionCookies)
    Q_ENUMS(ENUM_CookieKeepPolicy)
    Q_ENUMS(ENUM_CookieAcceptPolicy)
    */
};
//-----------------------------------------------------
#endif // FKCOOKIEJARMGR_H
