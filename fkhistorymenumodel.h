/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKHISTORYMENUMODEL_H
#define FKHISTORYMENUMODEL_H
//-----------------------------------------------------
#include <QtCore/QAbstractProxyModel>
//-----------------------------------------------------
class CFKHistoryTreeModel;
//-----------------------------------------------------
class CFKHistoryMenuModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    CFKHistoryMenuModel(CFKHistoryTreeModel* pSrcModel, QObject* pParent = NULL);
public:
    int         columnCount(const QModelIndex &parent) const;
    int         rowCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex mapFromSource(const QModelIndex & sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex & proxyIndex) const;
    QModelIndex index(int nRow, int nColumn, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index = QModelIndex()) const;
public:
    int         GetBumpedRows() const;
private:
    CFKHistoryTreeModel*        m_pTreeModel;
};
//-----------------------------------------------------
#endif // FKHISTORYMENUMODEL_H
