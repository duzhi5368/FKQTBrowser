/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKHISTORYTREEMODEL_H
#define FKHISTORYTREEMODEL_H
//-----------------------------------------------------
#include <QtCore/QAbstractProxyModel>
//-----------------------------------------------------
class CFKHistoryTreeModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    CFKHistoryTreeModel(QAbstractItemModel* pSourceModel, QObject* pParent = NULL);
public:
    QVariant        data(const QModelIndex &index, int nRole = Qt::DisplayRole) const;
    int             columnCount(const QModelIndex &parent) const;
    int             rowCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex     mapFromSource(const QModelIndex &sourceIndex) const;
    QModelIndex     mapToSource(const QModelIndex &proxyIndex) const;
    QModelIndex     index(int nRow, int nColumn, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex     parent(const QModelIndex &index= QModelIndex()) const;
    bool            hasChildren(const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags   flags(const QModelIndex &index) const;
    bool            removeRows(int nRow, int nCount, const QModelIndex &parent = QModelIndex());
    QVariant        headerData(int nSection, Qt::Orientation eOrientation, int nRole = Qt::DisplayRole) const;
    void            setSourceModel(QAbstractItemModel* pSourceModel);
private slots:
    void            __SLOT_SourceReset();
    void            __SLOT_SourceRowsInserted(const QModelIndex &parent, int nStart, int nEnd);
    void            __SLOT_SourceRowsRemoved(const QModelIndex &parent, int nStart, int nEnd);
    void            __SLOT_SourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
                           const QVector<int> vecRoles);
private:
    int             __GetSourceDateRow(int nRow) const;
private:
    mutable QList<int>      m_ListSourceRowCache;
};
//-----------------------------------------------------
#endif // FKHISTORYTREEMODEL_H
