/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include "fkaddbookmarkproxymodel.h"
//-----------------------------------------------------
CFKAddBookmarkProxyModel::CFKAddBookmarkProxyModel(QObject* pParent)
    : QSortFilterProxyModel(pParent)
{

}
//-----------------------------------------------------
int CFKAddBookmarkProxyModel::columnCount(const QModelIndex& parent) const
{
    return qMin(1, QSortFilterProxyModel::columnCount(parent));
}
//-----------------------------------------------------
bool CFKAddBookmarkProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex idx = sourceModel()->index(source_row, 0, source_parent);
    return sourceModel()->hasChildren(idx);
}
//-----------------------------------------------------
