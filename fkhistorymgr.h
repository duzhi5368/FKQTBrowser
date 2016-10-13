/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKHISTORYMGR_H
#define FKHISTORYMGR_H
//-----------------------------------------------------
#include <QtCore/QTimer>
#include <QtCore/QUrl>

#include "fkhistoryitem.h"
//-----------------------------------------------------
class CFKAutoSaver;
class CFKHistoryModel;
class CFKHistoryFilterModel;
class CFKHistoryTreeModel;
//-----------------------------------------------------
class CFKHistoryMgr : public QObject
{
    Q_OBJECT
signals:
    void                    SIGNAL_HistoryReset();
    void                    SIGNAL_EntryAdded(const CFKHistoryItem &item);
    void                    SIGNAL_EntryRemoved(const CFKHistoryItem &item);
    void                    SIGNAL_EntryUpdated(int nOffset);
public:
    CFKHistoryMgr(QObject* pParent = NULL);
    ~CFKHistoryMgr();
public:
    bool                    IsHistoryContains(const QString& url)const;
    void                    AddHistoryEntry(const QString& url);
    void                    RemoveHistoryEntry(const QString& url);
    void                    UpdateHistoryItem(const QUrl& url, const QString &title);
    int                     GetMaxHistoryLimit()const;
    void                    SetMaxHistoryLimit(int nLimit);
    QList<CFKHistoryItem>&  GetHistorys();
    void                    SetHistorys(const QList<CFKHistoryItem>& listHistorys,
                                        bool bIsLoadedAndSorted = false);
    CFKHistoryModel*        GetHistoryModel() const;
    CFKHistoryFilterModel*  GetHistoryFilterModel() const;
    CFKHistoryTreeModel*    GetHistoryTreeModel() const;
public slots:
    void                    SLOT_Clear();
    void                    SLOT_LoadSettings();
private slots:
    void                    __SLOT_Save();
    void                    __SLOT_CheckForExpired();
private:
    void                    __AddHistoryItem(const CFKHistoryItem& item);
    void                    __RemoveHistoryItem(const CFKHistoryItem& item);
    void                    __Load();
private:
    CFKAutoSaver*           m_pAutoSaver;
    int                     m_nMaxHistoryLimit;
    QTimer                  m_ExpiredTimer;
    QList<CFKHistoryItem>   m_ListHistorys;
    QString                 m_strLastSavedUrl;
    CFKHistoryModel*        m_pHistoryModel;
    CFKHistoryFilterModel*  m_pHistoryFilterModel;
    CFKHistoryTreeModel*    m_pHistoryTreeModel;
};
//-----------------------------------------------------
#endif // FKHISTORYMGR_H
