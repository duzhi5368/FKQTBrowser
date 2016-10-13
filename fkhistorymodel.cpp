/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtCore/QFileInfo>

#include "fkhistorymodel.h"
#include "fkhistorymgr.h"
#include "fkbrowserapp.h"
#include "fkhistoryitem.h"
//-----------------------------------------------------
void CFKHistoryModel::SLOT_HistoryReset()
{
    beginResetModel();
    m_pHistory->GetHistorys().clear();
    endResetModel();
}
//-----------------------------------------------------
void CFKHistoryModel::SLOT_EntryAdded(const CFKHistoryItem& item)
{
    beginInsertRows(QModelIndex(), 0, 0);
    m_pHistory->GetHistorys().prepend(item);
    endInsertRows();
}
//-----------------------------------------------------
void CFKHistoryModel::SLOT_EntryRemoved(const CFKHistoryItem& item)
{
    int nIndex = m_pHistory->GetHistorys().indexOf(item);
    Q_ASSERT(nIndex > -1);
    beginRemoveRows(QModelIndex(),nIndex, nIndex);
    m_pHistory->GetHistorys().takeAt(nIndex);
    endRemoveRows();
}
//-----------------------------------------------------
void CFKHistoryModel::SLOT_EntryUpdated(int nOffset)
{
    QModelIndex idx = index(nOffset, 0);
    emit dataChanged(idx, idx);
}
//-----------------------------------------------------
CFKHistoryModel::CFKHistoryModel(CFKHistoryMgr* pHistroy, QObject* pParent)
    : QAbstractTableModel(pParent)
    , m_pHistory(pHistroy)
{
    Q_ASSERT(m_pHistory);
    connect(m_pHistory, SIGNAL(SIGNAL_HistoryReset()),
            this, SLOT(SLOT_HistoryReset()));
    connect(m_pHistory, SIGNAL(SIGNAL_EntryRemoved(CFKHistoryItem)),
            this, SLOT(SLOT_EntryRemoved(CFKHistoryItem)));

    connect(m_pHistory, SIGNAL(SIGNAL_EntryAdded(CFKHistoryItem)),
            this, SLOT(SLOT_EntryAdded(CFKHistoryItem)));
    connect(m_pHistory, SIGNAL(SIGNAL_EntryUpdated(int)),
            this, SLOT(SLOT_EntryUpdated(int)));
}
//-----------------------------------------------------
QVariant CFKHistoryModel::headerData(int nSection, Qt::Orientation eOrientation, int nRole) const
{
    if (eOrientation == Qt::Horizontal
        && nRole == Qt::DisplayRole)
    {
        switch (nSection)
        {
            case 0: return tr("Title");
            case 1: return tr("Address");
        }
    }
    return QAbstractTableModel::headerData(nSection, eOrientation, nRole);
}
//-----------------------------------------------------
QVariant CFKHistoryModel::data(const QModelIndex &index, int nRole) const
{
    QList<CFKHistoryItem> lst = m_pHistory->GetHistorys();
    if (index.row() < 0 || index.row() >= lst.size())
    {
        return QVariant();
    }

    const CFKHistoryItem &item = lst.at(index.row());
    switch (nRole)
    {
    case eDateTimeRole:
        return item.m_tagDateTime;
    case eDateRole:
        return item.m_tagDateTime.date();
    case eUrlRole:
        return QUrl(item.m_strUrl);
    case eUrlStringRole:
        return item.m_strUrl;
    case Qt::DisplayRole:
    case Qt::EditRole:
        {
            switch (index.column())
            {
                case 0:
                    {
                        // 如果没有标题，则尝试通过Url生成一个
                        if (item.m_strTitle.isEmpty())
                        {
                            QString strPage = QFileInfo(QUrl(item.m_strUrl).path()).fileName();
                            if (!strPage.isEmpty())
                            {
                                return strPage;
                            }
                            return item.m_strUrl;
                        }
                        return item.m_strTitle;
                    }
                    break;
                case 1:
                    return item.m_strUrl;
                    break;
            }
        }
    case Qt::DecorationRole:
        if (index.column() == 0)
        {
            return CFKBrowserApp::GetAppInstancePtr()->GetIcon(item.m_strUrl);
        }
    }
    return QVariant();
}
//-----------------------------------------------------
int CFKHistoryModel::columnCount(const QModelIndex &parent) const
{
    return (parent.isValid()) ? 0 : 2;
}
//-----------------------------------------------------
int CFKHistoryModel::rowCount(const QModelIndex &parent) const
{
    return (parent.isValid()) ? 0 : m_pHistory->GetHistorys().count();
}
//-----------------------------------------------------
bool CFKHistoryModel::removeRows(int nRow, int nCount, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;

    int nLastRow = nRow + nCount - 1;
    beginRemoveRows(parent, nRow, nLastRow);
    QList<CFKHistoryItem> &lst = m_pHistory->GetHistorys();
    for (int i = nLastRow; i >= nRow; --i)
    {
        lst.removeAt(i);
    }
    endRemoveRows();
    return true;
}
//-----------------------------------------------------
