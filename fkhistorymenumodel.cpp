/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include "fkhistorymenumodel.h"
#include "fkhistorytreemodel.h"
//-----------------------------------------------------
const static int MAX_MOVE_DROWS_NUM = 15;
//-----------------------------------------------------
CFKHistoryMenuModel::CFKHistoryMenuModel(CFKHistoryTreeModel* pSrcModel, QObject* pParent)
    : QAbstractProxyModel(pParent)
    , m_pTreeModel(pSrcModel)
{
    setSourceModel(pSrcModel);
}
//-----------------------------------------------------
int CFKHistoryMenuModel::columnCount(const QModelIndex &parent) const
{
    return m_pTreeModel->columnCount(mapToSource(parent));
}
//-----------------------------------------------------
int CFKHistoryMenuModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
    {
        return 0;
    }

    if (!parent.isValid())
    {
        int nFolders = sourceModel()->rowCount();
        int nBumpedItems = GetBumpedRows();
        if (nBumpedItems <= MAX_MOVE_DROWS_NUM
            && nBumpedItems == sourceModel()->rowCount(sourceModel()->index(0, 0)))
        {
            --nFolders;
        }
        return nBumpedItems + nFolders;
    }

    if (parent.internalId() == quintptr(-1))
    {
        if (parent.row() < GetBumpedRows())
        {
            return 0;
        }
    }

    QModelIndex idx = mapToSource(parent);
    int nDefaultCount = sourceModel()->rowCount(idx);
    if (idx == sourceModel()->index(0, 0))
    {
        return nDefaultCount - GetBumpedRows();
    }
    return nDefaultCount;
}
//-----------------------------------------------------
QModelIndex CFKHistoryMenuModel::mapFromSource(const QModelIndex & sourceIndex) const
{
    Q_ASSERT(false);
    Q_UNUSED(sourceIndex);
    return QModelIndex();
    /*
    int sr = m_pTreeModel->mapToSource(sourceIndex).row();
    return createIndex(sourceIndex.row(), sourceIndex.column(), sr);
    */
}
//-----------------------------------------------------
QModelIndex CFKHistoryMenuModel::mapToSource(const QModelIndex & proxyIndex) const
{
    if (!proxyIndex.isValid())
    {
        return QModelIndex();
    }

    if (proxyIndex.internalId() == quintptr(-1))
    {
        int nBumpedItems = GetBumpedRows();
        if (proxyIndex.row() < nBumpedItems)
        {
            return m_pTreeModel->index(proxyIndex.row(), proxyIndex.column(), m_pTreeModel->index(0, 0));
        }
        if (nBumpedItems <= MAX_MOVE_DROWS_NUM && nBumpedItems == sourceModel()->rowCount(m_pTreeModel->index(0, 0)))
        {
            --nBumpedItems;
        }
        return m_pTreeModel->index(proxyIndex.row() - nBumpedItems, proxyIndex.column());
    }

    QModelIndex historyIndex    = m_pTreeModel->sourceModel()->index(proxyIndex.internalId(), proxyIndex.column());
    QModelIndex treeIndex       = m_pTreeModel->mapFromSource(historyIndex);
    return treeIndex;
}
//-----------------------------------------------------
QModelIndex CFKHistoryMenuModel::index(int nRow, int nColumn, const QModelIndex &parent) const
{
    if (nRow < 0
        || nColumn < 0 || nColumn >= columnCount(parent)
        || parent.column() > 0)
    {
        return QModelIndex();
    }
    if (!parent.isValid())
    {
        return createIndex(nRow, nColumn, quintptr(-1));
    }

    QModelIndex treeIndexParent = mapToSource(parent);

    int nBumpedItems = 0;
    if (treeIndexParent == m_pTreeModel->index(0, 0))
    {
        nBumpedItems = GetBumpedRows();
    }
    QModelIndex treeIndex = m_pTreeModel->index(nRow + nBumpedItems, nColumn, treeIndexParent);
    QModelIndex historyIndex = m_pTreeModel->mapToSource(treeIndex);
    int nHistoryRow = historyIndex.row();
    if (nHistoryRow == -1)
    {
        nHistoryRow = treeIndex.row();
    }
    return createIndex(nRow, nColumn, nHistoryRow);
}
//-----------------------------------------------------
QModelIndex CFKHistoryMenuModel::parent(const QModelIndex &index) const
{
    int nOffset = index.internalId();
    if (nOffset == -1 || !index.isValid())
    {
        return QModelIndex();
    }

    QModelIndex historyIndex    = m_pTreeModel->sourceModel()->index(index.internalId(), 0);
    QModelIndex treeIndex       = m_pTreeModel->mapFromSource(historyIndex);
    QModelIndex treeIndexParent = treeIndex.parent();

    int nSrc = m_pTreeModel->mapToSource(treeIndexParent).row();
    int nBumpedItems = GetBumpedRows();
    if (nBumpedItems <= MAX_MOVE_DROWS_NUM
            && nBumpedItems == sourceModel()->rowCount(sourceModel()->index(0, 0)))
    {
        --nBumpedItems;
    }
    return createIndex(nBumpedItems + treeIndexParent.row(), treeIndexParent.column(), nSrc);
}
//-----------------------------------------------------
int CFKHistoryMenuModel::GetBumpedRows() const
{
    QModelIndex first = m_pTreeModel->index(0, 0);
    if (!first.isValid())
    {
        return 0;
    }
    return qMin(m_pTreeModel->rowCount(first), MAX_MOVE_DROWS_NUM);
}
//-----------------------------------------------------
