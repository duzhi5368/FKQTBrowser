/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include "fktreeproxymodel.h"
#include "fkhistorymodel.h"
//-----------------------------------------------------
CFKTreeProxyModel::CFKTreeProxyModel(QObject* pParent)
    : QSortFilterProxyModel(pParent)
{
    setSortRole(CFKHistoryModel::eDateTimeRole);
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}
//-----------------------------------------------------
bool CFKTreeProxyModel::filterAcceptsRow(int nSourceRow, const QModelIndex &source_parent) const
{
    return QSortFilterProxyModel::filterAcceptsRow(nSourceRow, source_parent);
}
//-----------------------------------------------------
