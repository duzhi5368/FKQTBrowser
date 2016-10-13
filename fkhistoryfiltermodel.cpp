/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include "fkhistoryfiltermodel.h"
#include "fkhistorymodel.h"
//-----------------------------------------------------
CFKHistoryFilterModel::CFKHistoryFilterModel(QAbstractItemModel* pSrcModel, QObject* pParent)
    : QAbstractProxyModel(pParent)
{
    setSourceModel(pSrcModel);
}
//-----------------------------------------------------
bool CFKHistoryFilterModel::IsHistoryContains(const QString& url) const
{
    __Load();
    return m_HistorysHash.contains(url);
}
//-----------------------------------------------------
QModelIndex CFKHistoryFilterModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    QString strUrl = sourceIndex.data(CFKHistoryModel::eUrlStringRole).toString();
    if (!m_HistorysHash.contains(strUrl))
    {
        return QModelIndex();
    }

    int nRealRow        = -1;
    int nSourceModelRow = sourceModel()->rowCount() - sourceIndex.row();

    for (int i = 0; i < m_ListSourceRow.count(); ++i)
    {
        if (m_ListSourceRow.at(i) == nSourceModelRow)
        {
            nRealRow = i;
            break;
        }
    }
    if (nRealRow == -1)
    {
        return QModelIndex();
    }

    return createIndex(nRealRow, sourceIndex.column(), sourceModel()->rowCount() - sourceIndex.row());
}
//-----------------------------------------------------
QModelIndex CFKHistoryFilterModel::mapToSource(const QModelIndex &proxyIndex) const
{
    int nSourceRow = sourceModel()->rowCount() - proxyIndex.internalId();
    return sourceModel()->index(nSourceRow, proxyIndex.column());
}
//-----------------------------------------------------
void CFKHistoryFilterModel::setSourceModel(QAbstractItemModel* pNewSourceModel)
{
    beginResetModel();
    if (sourceModel())
    {
        disconnect(sourceModel(), SIGNAL(modelReset()),
                    this, SLOT(__SLOT_SourceReset()));
        disconnect(sourceModel(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                    this, SLOT(__SLOT_SourceDataChanged(QModelIndex,QModelIndex)));
        disconnect(sourceModel(), SIGNAL(rowsInserted(QModelIndex,int,int)),
                    this, SLOT(__SLOT_SourceRowsInserted(QModelIndex,int,int)));
        disconnect(sourceModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
                    this, SLOT(__SLOT_SourceRowsRemoved(QModelIndex,int,int)));
    }

    QAbstractProxyModel::setSourceModel(pNewSourceModel);

    if (sourceModel())
    {
        connect(sourceModel(), SIGNAL(modelReset()),
                this, SLOT(__SLOT_SourceReset()));
        connect(sourceModel(), SIGNAL(dataChanged(QModelIndex,QModelIndex)),
                this, SLOT(__SLOT_SourceDataChanged(QModelIndex,QModelIndex)));
        connect(sourceModel(), SIGNAL(rowsInserted(QModelIndex,int,int)),
                this, SLOT(__SLOT_SourceRowsInserted(QModelIndex,int,int)));
        connect(sourceModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
                this, SLOT(__SLOT_SourceRowsRemoved(QModelIndex,int,int)));
    }

    __Load();
    endResetModel();
}
//-----------------------------------------------------
QVariant CFKHistoryFilterModel::headerData(int nSection, Qt::Orientation eOrientation, int nRole) const
{
    return sourceModel()->headerData(nSection, eOrientation, nRole);
}
//-----------------------------------------------------
int CFKHistoryFilterModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_HistorysHash.count();
}
//-----------------------------------------------------
int CFKHistoryFilterModel::columnCount(const QModelIndex &parent) const
{
    return (parent.isValid()) ? 0 : 2;
}
//-----------------------------------------------------
QModelIndex CFKHistoryFilterModel::index(int nRow, int nColumn, const QModelIndex& parent) const
{
    if (nRow < 0 || nRow >= rowCount(parent)
        || nColumn < 0 || nColumn >= columnCount(parent))
    {
        return QModelIndex();
    }

    return createIndex(nRow, nColumn, m_ListSourceRow[nRow]);
}
//-----------------------------------------------------
QModelIndex CFKHistoryFilterModel::parent(const QModelIndex& index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}
//-----------------------------------------------------
bool CFKHistoryFilterModel::removeRows(int nRow, int nCount, const QModelIndex &parent)
{
    if (nRow < 0 || nCount <= 0
            || nRow + nCount > rowCount(parent)
            || parent.isValid())
    {
        return false;
    }
    int nLastRow = nRow + nCount - 1;
    disconnect(sourceModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
                this, SLOT(__SLOT_SourceRowsRemoved(QModelIndex,int,int)));

    beginRemoveRows(parent, nRow, nLastRow);
    int nOldCount   = rowCount();
    int nStart      = sourceModel()->rowCount() - m_ListSourceRow.value(nRow);
    int nEnd        = sourceModel()->rowCount() - m_ListSourceRow.value(nLastRow);
    sourceModel()->removeRows(nStart, nEnd - nStart + 1);
    endRemoveRows();

    connect(sourceModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
                this, SLOT(__SLOT_SourceRowsRemoved(QModelIndex,int,int)));

    if (nOldCount - nCount != rowCount())
    {
        beginResetModel();
        endResetModel();
    }
    return true;
}
//-----------------------------------------------------
void CFKHistoryFilterModel::__SLOT_SourceReset()
{
    beginResetModel();
    __Load();
    endResetModel();
}
//-----------------------------------------------------
void CFKHistoryFilterModel::__SLOT_SourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
     emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight));
}
//-----------------------------------------------------
void CFKHistoryFilterModel::__SLOT_SourceRowsInserted(const QModelIndex &parent, int nStart, int nEnd)
{
    Q_ASSERT(nStart == nEnd && nStart == 0);
    Q_UNUSED(nEnd);

    QModelIndex idx     = sourceModel()->index(nStart, 0, parent);
    QString strUrl      = idx.data(CFKHistoryModel::eUrlStringRole).toString();
    if (m_HistorysHash.contains(strUrl))
    {
        int nSourceRow = sourceModel()->rowCount() - m_HistorysHash[strUrl];
        int nRealRow = mapFromSource(sourceModel()->index(nSourceRow, 0)).row();

        beginRemoveRows(QModelIndex(), nRealRow, nRealRow);
        m_ListSourceRow.removeAt(nRealRow);
        m_HistorysHash.remove(strUrl);
        endRemoveRows();
    }

    beginInsertRows(QModelIndex(), 0, 0);
    m_HistorysHash.insert(strUrl, sourceModel()->rowCount() - nStart);
    m_ListSourceRow.insert(0, sourceModel()->rowCount());
    endInsertRows();
}
//-----------------------------------------------------
void CFKHistoryFilterModel::__SLOT_SourceRowsRemoved(const QModelIndex &noUse, int nStart, int nEnd)
{
    Q_UNUSED(nStart);
    Q_UNUSED(nEnd);
    Q_UNUSED(noUse);
    __SLOT_SourceReset();
}
//-----------------------------------------------------
void CFKHistoryFilterModel::__Load() const
{
    m_ListSourceRow.clear();
    m_HistorysHash.clear();
    m_HistorysHash.reserve(sourceModel()->rowCount());

    for (int i = 0; i < sourceModel()->rowCount(); ++i)
    {
        QModelIndex idx = sourceModel()->index(i, 0);
        QString strUrl = idx.data(CFKHistoryModel::eUrlStringRole).toString();
        if (!m_HistorysHash.contains(strUrl))
        {
            m_ListSourceRow.append(sourceModel()->rowCount() - i);
            m_HistorysHash[strUrl] = sourceModel()->rowCount() - i;
        }
    }
}
//-----------------------------------------------------
