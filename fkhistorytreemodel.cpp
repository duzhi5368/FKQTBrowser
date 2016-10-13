/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtGui/QIcon>
//-----------------------------------------------------
#include "fkhistorytreemodel.h"
#include "fkhistorymodel.h"
//-----------------------------------------------------
CFKHistoryTreeModel::CFKHistoryTreeModel(QAbstractItemModel* pSourceModel,
                                         QObject* pParent)
    : QAbstractProxyModel(pParent)
{
    setSourceModel(pSourceModel);
}
//-----------------------------------------------------
QVariant CFKHistoryTreeModel::data(const QModelIndex &index, int nRole) const
{
    if ((nRole == Qt::EditRole || nRole == Qt::DisplayRole))
    {
        int nStart = index.internalId();
        if (nStart == 0)
        {
            int nOffset = __GetSourceDateRow(index.row());
            if (index.column() == 0)
            {
                QModelIndex idx = sourceModel()->index(nOffset, 0);
                QDate date = idx.data(CFKHistoryModel::eDateRole).toDate();
                if (date == QDate::currentDate())
                {
                    return tr("Earlier Today");
                }
                return date.toString(QLatin1String("dddd, MMMM d, yyyy"));
            }
            if (index.column() == 1)
            {
                return tr("%1 items").arg(rowCount(index.sibling(index.row(), 0)));
            }
        }
    }
    if (nRole == Qt::DecorationRole && index.column() == 0
            && !index.parent().isValid())
    {
        return QIcon(QLatin1String(":history.png"));
    }
    if (nRole == CFKHistoryModel::eDateRole && index.column() == 0
            && index.internalId() == 0)
    {
        int nOffset = __GetSourceDateRow(index.row());
        QModelIndex idx = sourceModel()->index(nOffset, 0);
        return idx.data(CFKHistoryModel::eDateRole);
    }

    return QAbstractProxyModel::data(index, nRole);
}
//-----------------------------------------------------
int CFKHistoryTreeModel::columnCount(const QModelIndex &parent) const
{
    return sourceModel()->columnCount(mapToSource(parent));
}
//-----------------------------------------------------
int CFKHistoryTreeModel::rowCount(const QModelIndex &parent) const
{
    if ( parent.internalId() != 0
        || parent.column() > 0
        || !sourceModel())
    {
        return 0;
    }

    if (!parent.isValid())
    {
        if (!m_ListSourceRowCache.isEmpty())
        {
            return m_ListSourceRowCache.count();
        }
        QDate currentDate;
        int nRows = 0;
        int nTotalRows = sourceModel()->rowCount();

        for (int i = 0; i < nTotalRows; ++i)
        {
            QDate rowDate = sourceModel()->index(i, 0).data(CFKHistoryModel::eDateRole).toDate();
            if (rowDate != currentDate)
            {
                m_ListSourceRowCache.append(i);
                currentDate = rowDate;
                ++nRows;
            }
        }
        Q_ASSERT(m_ListSourceRowCache.count() == nRows);
        return nRows;
    }

    int nStart  = __GetSourceDateRow(parent.row());
    int nEnd    = __GetSourceDateRow(parent.row() + 1);
    return (nEnd - nStart);
}
//-----------------------------------------------------
QModelIndex CFKHistoryTreeModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceIndex.isValid())
    {
        return QModelIndex();
    }

    if (m_ListSourceRowCache.isEmpty())
    {
        rowCount(QModelIndex());
    }

    QList<int>::iterator Ite;
    Ite = qLowerBound(m_ListSourceRowCache.begin(), m_ListSourceRowCache.end(), sourceIndex.row());
    if (*Ite != sourceIndex.row())
    {
        --Ite;
    }
    int nDateRow    = qMax(0, Ite - m_ListSourceRowCache.begin());
    int nRow        = sourceIndex.row() - m_ListSourceRowCache.at(nDateRow);
    return createIndex(nRow, sourceIndex.column(), nDateRow + 1);
}
//-----------------------------------------------------
QModelIndex CFKHistoryTreeModel::mapToSource(const QModelIndex &proxyIndex) const
{
    int nOffset = proxyIndex.internalId();
    if (nOffset == 0)
    {
        return QModelIndex();
    }
    int nStartDateRow = __GetSourceDateRow(nOffset - 1);
    return sourceModel()->index(nStartDateRow + proxyIndex.row(), proxyIndex.column());
}
//-----------------------------------------------------
QModelIndex CFKHistoryTreeModel::index(int nRow, int nColumn, const QModelIndex &parent) const
{
    if (nRow < 0
        || nColumn < 0 || nColumn >= columnCount(parent)
        || parent.column() > 0)
    {
        return QModelIndex();
    }

    if (!parent.isValid())
    {
        return createIndex(nRow, nColumn);
    }
    return createIndex(nRow, nColumn, parent.row() + 1);
}
//-----------------------------------------------------
QModelIndex CFKHistoryTreeModel::parent(const QModelIndex &index) const
{
    int nOffset = index.internalId();
    if (nOffset == 0 || !index.isValid())
    {
        return QModelIndex();
    }
    return createIndex(nOffset - 1, 0);
}
//-----------------------------------------------------
bool CFKHistoryTreeModel::hasChildren(const QModelIndex &parent) const
{
    QModelIndex grandparent = parent.parent();
    if (!grandparent.isValid())
        return true;
    return false;
}
//-----------------------------------------------------
Qt::ItemFlags CFKHistoryTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled;
}
//-----------------------------------------------------
bool CFKHistoryTreeModel::removeRows(int nRow, int nCount, const QModelIndex &parent)
{
    if (nRow < 0 || nCount <= 0
            || nRow + nCount > rowCount(parent))
    {
        return false;
    }

    if (parent.isValid())
    {
        // 移除Page
        int nOffset = __GetSourceDateRow(parent.row());
        return sourceModel()->removeRows(nOffset + nRow, nCount);
    } else
    {
        // 移除全部数据
        for (int i = nRow + nCount - 1; i >= nRow; --i)
        {
            QModelIndex dateParent = index(i, 0);
            int nOffset = __GetSourceDateRow(dateParent.row());
            if (!sourceModel()->removeRows(nOffset, rowCount(dateParent)))
            {
                return false;
            }
        }
    }
    return true;
}
//-----------------------------------------------------
QVariant CFKHistoryTreeModel::headerData(int nSection, Qt::Orientation eOrientation, int nRole) const
{
    return sourceModel()->headerData(nSection, eOrientation, nRole);
}
//-----------------------------------------------------
void CFKHistoryTreeModel::setSourceModel(QAbstractItemModel* pNewSourceModel)
{
    beginResetModel();
    if (sourceModel())
    {
        disconnect(sourceModel(), SIGNAL(modelReset()),
                   this, SLOT(__SLOT_SourceReset()));
        disconnect(sourceModel(), SIGNAL(layoutChanged()),
                   this, SLOT(__SLOT_SourceReset()));
        disconnect(sourceModel(), SIGNAL(rowsInserted(QModelIndex,int,int)),
                this, SLOT(__SLOT_SourceRowsInserted(QModelIndex,int,int)));
        disconnect(sourceModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
                this, SLOT(__SLOT_SourceRowsRemoved(QModelIndex,int,int)));
        disconnect(sourceModel(), &QAbstractItemModel::dataChanged,
                   this, &CFKHistoryTreeModel::__SLOT_SourceDataChanged);
    }

    QAbstractProxyModel::setSourceModel(pNewSourceModel);

    if (pNewSourceModel)
    {
        connect(sourceModel(), SIGNAL(modelReset()),
                this, SLOT(__SLOT_SourceReset()));
        connect(sourceModel(), SIGNAL(layoutChanged()),
                this, SLOT(__SLOT_SourceReset()));
        connect(sourceModel(), SIGNAL(rowsInserted(QModelIndex,int,int)),
                this, SLOT(__SLOT_SourceRowsInserted(QModelIndex,int,int)));
        connect(sourceModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
                this, SLOT(__SLOT_SourceRowsRemoved(QModelIndex,int,int)));
        connect(sourceModel(), &QAbstractItemModel::dataChanged, this,
                &CFKHistoryTreeModel::__SLOT_SourceDataChanged);
    }
    endResetModel();
}
//-----------------------------------------------------
void CFKHistoryTreeModel::__SLOT_SourceReset()
{
    beginResetModel();
    m_ListSourceRowCache.clear();
    endResetModel();
}
//-----------------------------------------------------
void CFKHistoryTreeModel::__SLOT_SourceRowsInserted(const QModelIndex &parent, int nStart, int nEnd)
{
    Q_UNUSED(parent);
    Q_ASSERT(!parent.isValid());

    if (nStart != 0 || nStart != nEnd)
    {
        beginResetModel();
        m_ListSourceRowCache.clear();
        endResetModel();
        return;
    }

    m_ListSourceRowCache.clear();
    QModelIndex treeIndex = mapFromSource(sourceModel()->index(nStart, 0));
    QModelIndex treeParent = treeIndex.parent();
    if (rowCount(treeParent) == 1)
    {
        beginInsertRows(QModelIndex(), 0, 0);
        endInsertRows();
    }
    else
    {
        beginInsertRows(treeParent, treeIndex.row(), treeIndex.row());
        endInsertRows();
    }
}
//-----------------------------------------------------
void CFKHistoryTreeModel::__SLOT_SourceRowsRemoved(const QModelIndex &parent, int nStart, int nEnd)
{
    Q_UNUSED(parent);
    Q_ASSERT(!parent.isValid());

    if (m_ListSourceRowCache.isEmpty())
    {
        return;
    }
    for (int i = nEnd; i >= nStart; )
    {
        QList<int>::iterator Ite;
        Ite = qLowerBound(m_ListSourceRowCache.begin(), m_ListSourceRowCache.end(), i);

        if (Ite == m_ListSourceRowCache.end())
        {
            beginResetModel();
            m_ListSourceRowCache.clear();
            endResetModel();
            return;
        }

        if (*Ite != i)
        {
            --Ite;
        }
        int nRow    = qMax(0, Ite - m_ListSourceRowCache.begin());
        int nOffset = m_ListSourceRowCache[nRow];
        QModelIndex dateParent = index(nRow, 0);

        int nRowCount = rowCount(dateParent);
        if (i - nRowCount + 1 == nOffset
                && nStart <= i - nRowCount + 1)
        {
            beginRemoveRows(QModelIndex(), nRow, nRow);
            m_ListSourceRowCache.removeAt(nRow);
            i -= nRowCount + 1;
        }
        else
        {
            beginRemoveRows(dateParent, i - nOffset, i - nOffset);
            ++nRow;
            --i;
        }
        for (int j = nRow; j < m_ListSourceRowCache.count(); ++j)
        {
            --m_ListSourceRowCache[j];
        }
        endRemoveRows();
    }
}
//-----------------------------------------------------
void CFKHistoryTreeModel::__SLOT_SourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                       const QVector<int> vecRoles)
{
    emit dataChanged(mapFromSource(topLeft), mapFromSource(bottomRight), vecRoles);
}
//-----------------------------------------------------
int CFKHistoryTreeModel::__GetSourceDateRow(int nRow) const
{
    if (nRow <= 0)
        return 0;

    if (m_ListSourceRowCache.isEmpty())
    {
        rowCount(QModelIndex());
    }

    if (nRow >= m_ListSourceRowCache.count())
    {
        if (!sourceModel())
        {
            return 0;
        }
        return sourceModel()->rowCount();
    }
    return m_ListSourceRowCache.at(nRow);
}
//-----------------------------------------------------
