/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKADDBOOKMARKPROXYMODEL_H
#define FKADDBOOKMARKPROXYMODEL_H
//-----------------------------------------------------
#include <QtCore/QSortFilterProxyModel>
//-----------------------------------------------------
class CFKAddBookmarkProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    CFKAddBookmarkProxyModel(QObject* pParent = NULL);
public:
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};
//-----------------------------------------------------
#endif // FKADDBOOKMARKPROXYMODEL_H
