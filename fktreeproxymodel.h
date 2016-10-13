/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKTREEPROXYMODEL_H
#define FKTREEPROXYMODEL_H
//-----------------------------------------------------
#include <QtCore/QSortFilterProxyModel>
//-----------------------------------------------------
class CFKTreeProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    CFKTreeProxyModel(QObject* pParent = NULL);
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};
//-----------------------------------------------------
#endif // FKTREEPROXYMODEL_H
