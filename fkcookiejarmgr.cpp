/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtCore/QDir>
#include <QtCore/QSettings>
#include <QtCore/QStandardPaths>
#include <QtCore/QMetaEnum>
#include <QtCore/QCoreApplication>
#include <QtCore>

#include <QtNetwork/QNetworkCookie>

#include <QWebEngineSettings>

#include <QDebug>
//-----------------------------------------------------
#include "fkcookiejarmgr.h"
#include "fkautosaver.h"
//-----------------------------------------------------
static const unsigned int FK_COOKIE_JAR_VERSION = 1;
//-----------------------------------------------------
QT_BEGIN_NAMESPACE
QDataStream &operator<<(QDataStream &stream, const QList<QNetworkCookie> &list)
{
    stream << FK_COOKIE_JAR_VERSION;            // 版本号
    stream << quint32(list.size());             // 个数
    for (int i = 0; i < list.size(); ++i)       // 具体cookie信息
    {
        stream << list.at(i).toRawForm();
    }
    return stream;
}
//-----------------------------------------------------
QDataStream &operator>>(QDataStream &stream, QList<QNetworkCookie> &list)
{
    list.clear();

    quint32 unVersion = 0;                      // 版本号
    stream >> unVersion;

    if (unVersion != FK_COOKIE_JAR_VERSION)
    {   // Cookie版本不匹配
        return stream;
    }

    quint32 unCount = 0;
    stream >> unCount;                          // cookie个数

    for (quint32 i = 0; i < unCount; ++i)
    {
        QByteArray strValue;
        stream >> strValue;
        QList<QNetworkCookie> newCookies = QNetworkCookie::parseCookies(strValue);
        if (newCookies.count() == 0 && strValue.length() != 0)
        {
            qWarning() << "CFKCookieJarMgr: Unable to parse saved cookie:" << strValue;
        }
        for (int j = 0; j < newCookies.count(); ++j)
        {
            list.append(newCookies.at(j));
        }
        if (stream.atEnd())
        {
            break;
        }
    }
    return stream;
}
QT_END_NAMESPACE
//-----------------------------------------------------
CFKCookieJarMgr::CFKCookieJarMgr(QObject* pParent)
    : QNetworkCookieJar(pParent)
    , m_bIsLoaded(false)
    , m_eAcceptCookiePolicy(eCAP_AcceptAlways)
    , m_eKeepCookiePolicy(eCKP_KeepTimeLimit)
{
    m_pAutoSaver = new CFKAutoSaver(this);
}
//-----------------------------------------------------
CFKCookieJarMgr::~CFKCookieJarMgr()
{
    if(m_eKeepCookiePolicy == eCKP_KeepUntilExit)
        ClearCookie();

    m_pAutoSaver->SaveIfNeccessary();
}
//-----------------------------------------------------
// 清除Cookie
void CFKCookieJarMgr::ClearCookie()
{
    setAllCookies(QList<QNetworkCookie>());
    // 更新Cookie
    m_pAutoSaver->SLOT_OnChange();

    // 发送信号，Cookies被更改
    emit SIGNAL_CookiesChanged();
}
//-----------------------------------------------------
// 获取指定Url的Cookies
QList<QNetworkCookie> CFKCookieJarMgr::GetCookiesForUrl(const QUrl &url) const
{
    if(!m_bIsLoaded)
        (const_cast<CFKCookieJarMgr*>(this))->__LoadCookies();

    /*QWebEngineSettings* pGlobalSettings = QWebEngineSettings::globalSettings();
    if (pGlobalSettings->testAttribute(QWebEngineSettings::PrivateBrowsingEnabled))
    {
        // 获取失败，返回空列表
        QList<QNetworkCookie> emptyCookies;
        return emptyCookies;
    }*/

    return QNetworkCookieJar::cookiesForUrl(url);
}
//-----------------------------------------------------
// 设置指定Url的Cookies
bool CFKCookieJarMgr::SetCookiesFromUrl(const QList<QNetworkCookie> &cookieList,
                                          const QUrl &url)
{
    if(!m_bIsLoaded)
        __LoadCookies();

    /*QWebEngineSettings* pGlobalSettings = QWebEngineSettings::globalSettings();
    if (pGlobalSettings->testAttribute(QWebEngineSettings::PrivateBrowsingEnabled))
    {
        return false;
    }*/

    QString strHost         = url.host();
    bool bIsBlock           = qBinaryFind(m_ExceptionsBlockList.begin(), m_ExceptionsBlockList.end(), strHost) != m_ExceptionsBlockList.end();
    bool bIsAllow           = qBinaryFind(m_ExceptionsAllowList.begin(), m_ExceptionsAllowList.end(), strHost) != m_ExceptionsAllowList.end();
    bool bIsAllowSession    = qBinaryFind(m_ExceptionsAllowForSessionList.begin(), m_ExceptionsAllowForSessionList.end(), strHost) != m_ExceptionsAllowForSessionList.end();

    bool bIsAddedCookies    = false;
    bool bIsAcceptInitially = (m_eAcceptCookiePolicy != eCAP_AcceptAlways);
    if ((bIsAcceptInitially && !bIsBlock)
        || (!bIsAcceptInitially && (bIsAllow || bIsAllowSession)))
    {
        QDateTime tagLimitTime = QDateTime::currentDateTime();
        tagLimitTime = tagLimitTime.addDays(90);    // 90天后过期
        foreach (QNetworkCookie curCookie, cookieList)
        {
            QList<QNetworkCookie> listCookie;
            if (m_eKeepCookiePolicy == eCKP_KeepTimeLimit
                && !curCookie.isSessionCookie()
                && curCookie.expirationDate() > tagLimitTime)
            {
                curCookie.setExpirationDate(tagLimitTime);
            }
            listCookie += curCookie;
            if (QNetworkCookieJar::setCookiesFromUrl(listCookie, url))
            {
                bIsAddedCookies     = true;
            }
            else
            {
                if (m_eAcceptCookiePolicy == eCAP_AcceptAlways)
                {
                    QList<QNetworkCookie> cookies = allCookies();
                    cookies += curCookie;
                    setAllCookies(cookies);
                    bIsAddedCookies     = true;
                }
                else
                {
                    qWarning() << "setCookiesFromUrl failed" << url << cookieList.value(0).toRawForm();
                }
            }
        }
    }

    // 若需要添加苯Cookie
    if (bIsAddedCookies)
    {
        // 自动保存Cookies
        m_pAutoSaver->SLOT_OnChange();
        // 发送信号，Cookies被更改
        emit SIGNAL_CookiesChanged();
    }
    return bIsAddedCookies;
}
//-----------------------------------------------------
// 获取Cookie接受策略
CFKCookieJarMgr::ENUM_CookieAcceptPolicy CFKCookieJarMgr::GetAcceptPolicy() const
{
    if(!m_bIsLoaded)
        (const_cast<CFKCookieJarMgr*>(this))->__LoadCookies();
    return m_eAcceptCookiePolicy;
}
//-----------------------------------------------------
// 设置Cookie接受策略
void CFKCookieJarMgr::SetAcceptPolicy(ENUM_CookieAcceptPolicy ePolicy)
{
    if(!m_bIsLoaded)
        (const_cast<CFKCookieJarMgr*>(this))->__LoadCookies();
    if(m_eAcceptCookiePolicy == ePolicy)
        return;
    m_eAcceptCookiePolicy = ePolicy;
    // 通知定时记录
    m_pAutoSaver->SLOT_OnChange();
}
//-----------------------------------------------------
// 获取Cookie保持策略
CFKCookieJarMgr::ENUM_CookieKeepPolicy CFKCookieJarMgr::GetKeepPolicy() const
{
    if(!m_bIsLoaded)
        (const_cast<CFKCookieJarMgr*>(this))->__LoadCookies();
    return m_eKeepCookiePolicy;
}
//-----------------------------------------------------
// 设置Cookie保持策略
void CFKCookieJarMgr::SetKeepPolicy(ENUM_CookieKeepPolicy ePolicy)
{
    if(!m_bIsLoaded)
        __LoadCookies();
    if(m_eKeepCookiePolicy == ePolicy)
        return;
    m_eKeepCookiePolicy = ePolicy;
    // 通知定时记录
    m_pAutoSaver->SLOT_OnChange();
}
//-----------------------------------------------------
// 获取阻止的异常列表
QStringList CFKCookieJarMgr::GetBlockedCookies() const
{
    if(!m_bIsLoaded)
        (const_cast<CFKCookieJarMgr*>(this))->__LoadCookies();
    return m_ExceptionsBlockList;
}
//-----------------------------------------------------
// 设置阻止的异常列表
void CFKCookieJarMgr::SetBlockedCookies(const QStringList& list)
{
    if(!m_bIsLoaded)
        __LoadCookies();
    m_ExceptionsBlockList = list;
    qSort(m_ExceptionsBlockList.begin(), m_ExceptionsBlockList.end());
    // 通知定时记录
    m_pAutoSaver->SLOT_OnChange();
}
//-----------------------------------------------------
// 获取许可的异常列表
QStringList CFKCookieJarMgr::GetAllowedCookies() const
{
    if(!m_bIsLoaded)
        (const_cast<CFKCookieJarMgr*>(this))->__LoadCookies();
    return m_ExceptionsAllowList;
}
//-----------------------------------------------------
// 设置许可的异常列表
void CFKCookieJarMgr::SetAllowedCookies(const QStringList& list)
{
    if(!m_bIsLoaded)
        __LoadCookies();
    m_ExceptionsAllowList = list;
    qSort(m_ExceptionsAllowList.begin(), m_ExceptionsAllowList.end());
    // 通知定时记录
    m_pAutoSaver->SLOT_OnChange();
}
//-----------------------------------------------------
// 获取许可连接的异常列表
QStringList CFKCookieJarMgr::GetAllowForSessionCookies() const
{
    if(!m_bIsLoaded)
        (const_cast<CFKCookieJarMgr*>(this))->__LoadCookies();
    return m_ExceptionsAllowForSessionList;
}
//-----------------------------------------------------
// 设置许可连接的异常列表
void CFKCookieJarMgr::SetAllowForSessionCookies(const QStringList& list)
{
    if(!m_bIsLoaded)
        __LoadCookies();
    m_ExceptionsAllowForSessionList = list;
    qSort(m_ExceptionsAllowForSessionList.begin(), m_ExceptionsAllowForSessionList.end());
    // 通知定时记录
    m_pAutoSaver->SLOT_OnChange();
}
//-----------------------------------------------------
// 加载Cookie
void CFKCookieJarMgr::__LoadCookies()
{
    if(m_bIsLoaded)
        return;

    // 加载Cookies
    qRegisterMetaTypeStreamOperators<QList<QNetworkCookie> >("QList<QNetworkCookie>");

    QSettings tagCookieSettings(QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QLatin1String("/cookies.ini"), QSettings::IniFormat);
    setAllCookies(qvariant_cast<QList<QNetworkCookie> >(tagCookieSettings.value(QLatin1String("cookies"))));

    {   // 异常处理策略
        tagCookieSettings.beginGroup(QLatin1String("Exceptions"));
        m_ExceptionsBlockList              = tagCookieSettings.value(QLatin1String("block")).toStringList();
        m_ExceptionsAllowList              = tagCookieSettings.value(QLatin1String("allow")).toStringList();
        m_ExceptionsAllowForSessionList    = tagCookieSettings.value(QLatin1String("allowForSession")).toStringList();
        qSort(m_ExceptionsBlockList.begin(), m_ExceptionsBlockList.end());
        qSort(m_ExceptionsAllowList.begin(), m_ExceptionsAllowList.end());
        qSort(m_ExceptionsAllowForSessionList.begin(), m_ExceptionsAllowForSessionList.end());
        tagCookieSettings.endGroup();
    }

    QSettings tagSettings;
    {   // cookie许可和保存策略
        tagSettings.beginGroup(QLatin1String("cookies"));
        QByteArray strValue;
        strValue = tagSettings.value(QLatin1String("acceptCookies"),
                            QLatin1String("AcceptOnlyFromSitesNavigatedTo")).toByteArray();
        QMetaEnum acceptPolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("AcceptPolicy"));
        m_eAcceptCookiePolicy = acceptPolicyEnum.keyToValue(strValue) == -1 ?
                           eCAP_AcceptAlways :
                           static_cast<ENUM_CookieAcceptPolicy>(acceptPolicyEnum.keyToValue(strValue));

        strValue = tagSettings.value(QLatin1String("keepCookiesUntil"), QLatin1String("KeepUntilExpire")).toByteArray();
        QMetaEnum keepPolicyEnum = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("KeepPolicy"));
        m_eKeepCookiePolicy = keepPolicyEnum.keyToValue(strValue) == -1 ?
                            eCKP_KeepTimeLimit :
                            static_cast<ENUM_CookieKeepPolicy>(keepPolicyEnum.keyToValue(strValue));

        if (m_eKeepCookiePolicy == eCKP_KeepUntilExit)
            setAllCookies(QList<QNetworkCookie>());
        tagSettings.endGroup();
    }

    m_bIsLoaded = true;
    // 发送信号，Cookies被更改
    emit SIGNAL_CookiesChanged();
}
//-----------------------------------------------------
// 清除老Cookies
void CFKCookieJarMgr::__PurgeOldCookies()
{
    QList<QNetworkCookie> ListCookies = allCookies();
    if (ListCookies.isEmpty())
        return;

    int nOldCookiesCount = ListCookies.count();
    QDateTime tagCurTime = QDateTime::currentDateTime();
    for (int i = ListCookies.count() - 1; i >= 0; --i)
    {
        if (!ListCookies.at(i).isSessionCookie() &&
                ListCookies.at(i).expirationDate() < tagCurTime)
            ListCookies.removeAt(i);
    }
    if (nOldCookiesCount == ListCookies.count())
    {   // 没有Cookies的变化
        return;
    }

    setAllCookies(ListCookies);
    // 发送信号，Cookies被更改
    emit SIGNAL_CookiesChanged();
}
//-----------------------------------------------------
// 保存Cookies
void CFKCookieJarMgr::SLOT_Save()
{
    if(!m_bIsLoaded)
        return;

    // 清除老Cookies
    __PurgeOldCookies();

    QString strDirectory = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    if (strDirectory.isEmpty())
    {
        strDirectory = QDir::homePath() + QLatin1String("/.") + QCoreApplication::applicationName();
    }
    if (!QFile::exists(strDirectory))
    {
        QDir dir;
        dir.mkpath(strDirectory);
    }
    QSettings tagCookieSettings(strDirectory + QLatin1String("/cookies.ini"), QSettings::IniFormat);
    QList<QNetworkCookie> ListCookies = allCookies();
    for (int i = ListCookies.count() - 1; i >= 0; --i)
    {
        if (ListCookies.at(i).isSessionCookie())
            ListCookies.removeAt(i);
    }
    tagCookieSettings.setValue(QLatin1String("cookies"), QVariant::fromValue<QList<QNetworkCookie> >(ListCookies));
    tagCookieSettings.beginGroup(QLatin1String("Exceptions"));
    tagCookieSettings.setValue(QLatin1String("block"), m_ExceptionsBlockList);
    tagCookieSettings.setValue(QLatin1String("allow"), m_ExceptionsAllowList);
    tagCookieSettings.setValue(QLatin1String("allowForSession"), m_ExceptionsAllowForSessionList);

    QSettings tagSettings;
    tagSettings.beginGroup(QLatin1String("cookies"));
    QMetaEnum enumAcceptPolicy = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("AcceptPolicy"));
    tagSettings.setValue(QLatin1String("acceptCookies"), QLatin1String(enumAcceptPolicy.valueToKey(m_eAcceptCookiePolicy)));

    QMetaEnum enumKeepPolicy = staticMetaObject.enumerator(staticMetaObject.indexOfEnumerator("KeepPolicy"));
    tagSettings.setValue(QLatin1String("keepCookiesUntil"), QLatin1String(enumKeepPolicy.valueToKey(m_eKeepCookiePolicy)));
}
//-----------------------------------------------------
