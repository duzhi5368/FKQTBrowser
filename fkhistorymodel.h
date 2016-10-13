/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKHISTORYMODEL_H
#define FKHISTORYMODEL_H
//-----------------------------------------------------
#include <QtCore/QVariant>
#include <QtCore/QAbstractItemModel>
//-----------------------------------------------------
#include "fkhistoryitem.h"
//-----------------------------------------------------
class CFKHistoryMgr;
//-----------------------------------------------------
class CFKHistoryModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum ENUM_Roles{
        eDateRole       = Qt::UserRole + 1,
        eDateTimeRole   = Qt::UserRole + 2,
        eUrlRole        = Qt::UserRole + 3,
        eUrlStringRole  = Qt::UserRole + 4
    };

public slots:
    void        SLOT_HistoryReset();
    void        SLOT_EntryAdded(const CFKHistoryItem& item);
    void        SLOT_EntryRemoved(const CFKHistoryItem& item);
    void        SLOT_EntryUpdated(int nOffset);
public:
    CFKHistoryModel(CFKHistoryMgr* pHistroy, QObject* pParent = NULL);
public:
    QVariant    headerData(int nSection, Qt::Orientation eOrientation, int nRole = Qt::DisplayRole) const;
    QVariant    data(const QModelIndex &index, int nRole = Qt::DisplayRole) const;
    int         columnCount(const QModelIndex &parent = QModelIndex()) const;
    int         rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool        removeRows(int nRow, int nCount, const QModelIndex &parent = QModelIndex());
private:
    CFKHistoryMgr*  m_pHistory;
};
//-----------------------------------------------------
#endif // FKHISTORYMODEL_H
