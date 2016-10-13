/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtCore/QUrl>
//-----------------------------------------------------
#include "fkhistorycompletionmodel.h"
#include "fkhistorymodel.h"
//-----------------------------------------------------
CFKHistoryCompletionModel::CFKHistoryCompletionModel(QObject* pParent)
    : QAbstractProxyModel(pParent)
{

}
//-----------------------------------------------------
QVariant CFKHistoryCompletionModel::data(const QModelIndex &index, int nRole) const
{
    if (sourceModel()
        && (nRole == Qt::EditRole || nRole == Qt::DisplayRole)
        && index.isValid())
    {
        QModelIndex idx = mapToSource(index);
        idx = idx.sibling(idx.row(), 1);
        QString strUrlString = idx.data(CFKHistoryModel::eUrlStringRole).toString();
        if (index.row() % 2)
        {
            QUrl url = strUrlString;
            QString str = url.toString(QUrl::RemoveScheme
                                     | QUrl::RemoveUserInfo
                                     | QUrl::StripTrailingSlash);
            return str.mid(2);  // strip // from the front
        }
        return strUrlString;
    }
    return QAbstractProxyModel::data(index, nRole);

}
//-----------------------------------------------------
int CFKHistoryCompletionModel::rowCount(const QModelIndex &parent) const
{
    return (parent.isValid()
            || !sourceModel()) ? 0 : sourceModel()->rowCount(parent) * 2;
}
//-----------------------------------------------------
int CFKHistoryCompletionModel::columnCount(const QModelIndex &parent) const
{
    return (parent.isValid()) ? 0 : 1;
}
//-----------------------------------------------------
QModelIndex CFKHistoryCompletionModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    int nRow = sourceIndex.row() * 2;
    return index(nRow, sourceIndex.column());
}
//-----------------------------------------------------
QModelIndex CFKHistoryCompletionModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!sourceModel())
    {
        return QModelIndex();
    }
    int nRow = proxyIndex.row() / 2;
    return sourceModel()->index(nRow, proxyIndex.column());
}
//-----------------------------------------------------
QModelIndex CFKHistoryCompletionModel::index(int nRow, int nColumn, const QModelIndex& parent) const
{
    if (nRow < 0 || nRow >= rowCount(parent)
        || nColumn < 0 || nColumn >= columnCount(parent))
    {
        return QModelIndex();
    }
    return createIndex(nRow, nColumn);
}
//-----------------------------------------------------
QModelIndex CFKHistoryCompletionModel::parent(const QModelIndex& index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}
//-----------------------------------------------------
void CFKHistoryCompletionModel::setSourceModel(QAbstractItemModel* pNewSourceModel)
{
    if (sourceModel())
    {
        disconnect(sourceModel(), SIGNAL(modelReset()),
                this, SLOT(__SLOT_SourceReset()));
        disconnect(sourceModel(), SIGNAL(rowsInserted(QModelIndex,int,int)),
                this, SLOT(__SLOT_SourceReset()));
        disconnect(sourceModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
                this, SLOT(__SLOT_SourceReset()));
    }

    QAbstractProxyModel::setSourceModel(pNewSourceModel);

    if (pNewSourceModel)
    {
        connect(pNewSourceModel, SIGNAL(modelReset()),
                this, SLOT(__SLOT_SourceReset()));
        connect(sourceModel(), SIGNAL(rowsInserted(QModelIndex,int,int)),
                this, SLOT(__SLOT_SourceReset()));
        connect(sourceModel(), SIGNAL(rowsRemoved(QModelIndex,int,int)),
                this, SLOT(__SLOT_SourceReset()));
    }

    beginResetModel();
    endResetModel();
}
//-----------------------------------------------------
void CFKHistoryCompletionModel::__SLOT_SourceReset()
{
    beginResetModel();
    endResetModel();
}
//-----------------------------------------------------
