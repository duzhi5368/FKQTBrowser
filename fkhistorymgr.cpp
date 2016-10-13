/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QDebug>
#include <QtCore/QUrl>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QBuffer>
#include <QtCore/QSettings>
#include <QtCore/QTemporaryFile>
#include <QtCore/QStandardPaths>
//-----------------------------------------------------
#include "fkhistorymgr.h"
#include "fkautosaver.h"
#include "fkbrowserapp.h"
#include "fkhistorymodel.h"
#include "fkhistorytreemodel.h"
#include "fkhistoryfiltermodel.h"
//-----------------------------------------------------
const static int DEFAULT_MAX_HISTORY_LIMIT = 30;
const static int HISTORYMGR_VERSION = 1;
//-----------------------------------------------------
CFKHistoryMgr::CFKHistoryMgr(QObject* pParent)
    : QObject(pParent)
    , m_nMaxHistoryLimit(DEFAULT_MAX_HISTORY_LIMIT)
    , m_pHistoryModel(NULL)
    , m_pHistoryFilterModel(NULL)
    , m_pHistoryTreeModel(NULL)
{
    m_pAutoSaver            = new CFKAutoSaver(this);
    m_pHistoryModel         = new CFKHistoryModel(this, this);
    m_pHistoryFilterModel   = new CFKHistoryFilterModel(m_pHistoryModel, this);
    m_pHistoryTreeModel     = new CFKHistoryTreeModel(m_pHistoryFilterModel, this);

    m_ExpiredTimer.setSingleShot(true);

    connect(&m_ExpiredTimer, SIGNAL(timeout()),
            this, SLOT(__SLOT_CheckForExpired()));
    connect(this, SIGNAL(SIGNAL_EntryAdded(CFKHistoryItem)),
            m_pAutoSaver, SLOT(SLOT_OnChange()));
    connect(this, SIGNAL(SIGNAL_EntryRemoved(CFKHistoryItem)),
            m_pAutoSaver, SLOT(SLOT_OnChange()));

    __Load();
}
//-----------------------------------------------------
CFKHistoryMgr::~CFKHistoryMgr()
{
    m_pAutoSaver->SaveIfNeccessary();
}
//-----------------------------------------------------
bool CFKHistoryMgr::IsHistoryContains(const QString& url)const
{
    return m_pHistoryFilterModel->IsHistoryContains(url);
}
//-----------------------------------------------------
void CFKHistoryMgr::AddHistoryEntry(const QString& url)
{
    QUrl cleanUrl(url);
    cleanUrl.setPassword(QString());
    cleanUrl.setHost(cleanUrl.host().toLower());
    CFKHistoryItem item(cleanUrl.toString(), QDateTime::currentDateTime());
    __AddHistoryItem(item);
}
//-----------------------------------------------------
void CFKHistoryMgr::RemoveHistoryEntry(const QString& url)
{
    QUrl cleanUrl(url);
    cleanUrl.setPassword(QString());
    cleanUrl.setHost(cleanUrl.host().toLower());
    CFKHistoryItem item(cleanUrl.toString(), QDateTime::currentDateTime());
    __RemoveHistoryItem(item);
}
//-----------------------------------------------------
void CFKHistoryMgr::UpdateHistoryItem(const QUrl& url, const QString &title)
{
    for (int i = 0; i < m_ListHistorys.count(); ++i)
    {
        if (url == m_ListHistorys.at(i).m_strUrl)
        {
            m_ListHistorys[i].m_strTitle = title;
            m_pAutoSaver->SLOT_OnChange();
            if (m_strLastSavedUrl.isEmpty())
            {
                m_strLastSavedUrl = m_ListHistorys.at(i).m_strUrl;
            }
            emit SIGNAL_EntryUpdated(i);
            break;
        }
    }
}
//-----------------------------------------------------
int CFKHistoryMgr::GetMaxHistoryLimit()const
{
    return m_nMaxHistoryLimit;
}
//-----------------------------------------------------
void CFKHistoryMgr::SetMaxHistoryLimit(int nLimit)
{
    if (m_nMaxHistoryLimit == nLimit)
        return;

    m_nMaxHistoryLimit = nLimit;
    __SLOT_CheckForExpired();
    m_pAutoSaver->SLOT_OnChange();
}
//-----------------------------------------------------
QList<CFKHistoryItem>& CFKHistoryMgr::GetHistorys()
{
    return m_ListHistorys;
}
//-----------------------------------------------------
void CFKHistoryMgr::SetHistorys(const QList<CFKHistoryItem>& listHistorys,
                                    bool bIsLoadedAndSorted)
{
    m_ListHistorys = listHistorys;

    if (!bIsLoadedAndSorted)
    {
        // 若没排序，则进行排序
        qSort(m_ListHistorys.begin(), m_ListHistorys.end());
    }

    __SLOT_CheckForExpired();

    if (bIsLoadedAndSorted)
    {
        m_strLastSavedUrl = m_ListHistorys.value(0).m_strUrl;
    }
    else
    {
        m_strLastSavedUrl = QString();
        m_pAutoSaver->SLOT_OnChange();
    }
    emit SIGNAL_HistoryReset();
}
//-----------------------------------------------------
CFKHistoryModel* CFKHistoryMgr::GetHistoryModel() const
{
    return m_pHistoryModel;
}
//-----------------------------------------------------
CFKHistoryFilterModel* CFKHistoryMgr::GetHistoryFilterModel() const
{
    return m_pHistoryFilterModel;
}
//-----------------------------------------------------
CFKHistoryTreeModel* CFKHistoryMgr::GetHistoryTreeModel() const
{
    return m_pHistoryTreeModel;
}
//-----------------------------------------------------
void CFKHistoryMgr::SLOT_Clear()
{
    m_strLastSavedUrl = QString();
    emit SIGNAL_HistoryReset();
    m_pAutoSaver->SLOT_OnChange();
    m_pAutoSaver->SaveIfNeccessary();
}
//-----------------------------------------------------
void CFKHistoryMgr::SLOT_LoadSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("history"));
    m_nMaxHistoryLimit = settings.value(QLatin1String("historyLimit"), 30).toInt();
}
//-----------------------------------------------------
void CFKHistoryMgr::__SLOT_Save()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("history"));
    settings.setValue(QLatin1String("historyLimit"), m_nMaxHistoryLimit);

    bool bIsSaveAll = m_strLastSavedUrl.isEmpty();
    int nFirst = m_ListHistorys.count() - 1;
    if (!bIsSaveAll)
    {
        for (int i = 0; i < m_ListHistorys.count(); ++i)
        {
            if (m_ListHistorys.at(i).m_strUrl == m_strLastSavedUrl)
            {
                nFirst = i - 1;
                break;
            }
        }
    }
    if (nFirst == m_ListHistorys.count() - 1)
    {
        bIsSaveAll = true;
    }

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

    QFile historyFile(strDirectory + QLatin1String("/history"));
    QTemporaryFile tempFile;
    tempFile.setAutoRemove(false);
    bool bIsOpen = false;
    if (bIsSaveAll)
    {
        bIsOpen = tempFile.open();
    }
    else
    {
        bIsOpen = historyFile.open(QFile::Append);
    }

    if (!bIsOpen)
    {
        qWarning() << "Unable to open history file for saving"
                   << (bIsSaveAll ? tempFile.fileName() : historyFile.fileName());
        return;
    }

    QDataStream out(bIsSaveAll ? &tempFile : &historyFile);
    for (int i = nFirst; i >= 0; --i)
    {
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        CFKHistoryItem item = m_ListHistorys.at(i);
        stream << HISTORYMGR_VERSION <<
                  item.m_strUrl << item.m_tagDateTime << item.m_strTitle;
        out << data;
    }
    tempFile.close();

    if (bIsSaveAll)
    {
        if (historyFile.exists() && !historyFile.remove())
        {
            qWarning() << "History: error removing old history." << historyFile.errorString();
        }
        if (!tempFile.rename(historyFile.fileName()))
        {
            qWarning() << "History: error moving new history over old." << tempFile.errorString() << historyFile.fileName();
        }
    }

    m_strLastSavedUrl = m_ListHistorys.value(0).m_strUrl;
}
//-----------------------------------------------------
void CFKHistoryMgr::__SLOT_CheckForExpired()
{
    if (m_nMaxHistoryLimit < 0 || m_ListHistorys.isEmpty())
        return;

    QDateTime tagCurTime = QDateTime::currentDateTime();
    int     nNextTimeout = 0;

    while (!m_ListHistorys.isEmpty())
    {
        QDateTime tagCheckForExpiredTime = m_ListHistorys.last().m_tagDateTime;
        tagCheckForExpiredTime.setDate(tagCheckForExpiredTime.date().addDays(m_nMaxHistoryLimit));
        if (tagCurTime.daysTo(tagCheckForExpiredTime) > 7)
        {
            // 增加7天
            nNextTimeout = 7 * 86400;
        }
        else
        {
            nNextTimeout = tagCurTime.secsTo(tagCheckForExpiredTime);
        }
        if (nNextTimeout > 0)
        {
            break;
        }

        const CFKHistoryItem& item = m_ListHistorys.last();
        m_strLastSavedUrl = QString();
        emit SIGNAL_EntryRemoved(item);
    }

    if (nNextTimeout > 0)
    {
        m_ExpiredTimer.start(nNextTimeout * 1000);
    }
}
//-----------------------------------------------------
void CFKHistoryMgr::__AddHistoryItem(const CFKHistoryItem& item)
{
    if (CFKBrowserApp::GetAppInstancePtr()->IsPrivateBrowsing())
        return;

    emit SIGNAL_EntryAdded(item);
    if (m_ListHistorys.count() == 1)
    {
        __SLOT_CheckForExpired();
    }
}
//-----------------------------------------------------
void CFKHistoryMgr::__RemoveHistoryItem(const CFKHistoryItem& item)
{
    for (int i = m_ListHistorys.count() - 1 ; i >= 0; --i)
    {
        if (item.m_strUrl == m_ListHistorys.at(i).m_strUrl)
        {
            emit SIGNAL_EntryRemoved(m_ListHistorys.at(i));
        }
    }
}
//-----------------------------------------------------
void CFKHistoryMgr::__Load()
{
    SLOT_LoadSettings();

    QFile historyFile(QStandardPaths::writableLocation(QStandardPaths::DataLocation)
        + QLatin1String("/history"));
    if (!historyFile.exists())
    {
        return;
    }
    if (!historyFile.open(QFile::ReadOnly))
    {
        qWarning() << "Unable to open history file" << historyFile.fileName();
        return;
    }

    QList<CFKHistoryItem> ListHistoryItems;
    QDataStream in(&historyFile);
    bool bIsNeedToSort = false;
    CFKHistoryItem  lastInsertedItem;
    QByteArray      strData;
    QDataStream     stream;
    QBuffer         buffer;
    stream.setDevice(&buffer);
    while (!historyFile.atEnd())
    {
        in >> strData;
        buffer.close();
        buffer.setBuffer(&strData);
        buffer.open(QIODevice::ReadOnly);
        quint32 nVersion;
        stream >> nVersion;
        if (nVersion != HISTORYMGR_VERSION)
        {
            continue;
        }
        CFKHistoryItem item;
        stream >> item.m_strUrl;
        stream >> item.m_tagDateTime;
        stream >> item.m_strTitle;

        if (!item.m_tagDateTime.isValid())
        {
            continue;
        }

        if (item == lastInsertedItem)
        {
            if (lastInsertedItem.m_strTitle.isEmpty()
                    && !ListHistoryItems.isEmpty())
            {
                ListHistoryItems[0].m_strTitle = item.m_strTitle;
            }
            continue;
        }

        if (!bIsNeedToSort && !ListHistoryItems.isEmpty()
                && lastInsertedItem < item)
        {
            bIsNeedToSort = true;
        }

        ListHistoryItems.prepend(item);
        lastInsertedItem = item;
    }
    if (bIsNeedToSort)
    {
        qSort(ListHistoryItems.begin(), ListHistoryItems.end());
    }

    SetHistorys(ListHistoryItems, true);

    if (bIsNeedToSort)
    {
        m_strLastSavedUrl = QString();
        m_pAutoSaver->SLOT_OnChange();
    }
}
//-----------------------------------------------------
