/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKHISTORYFILTERMODEL_H
#define FKHISTORYFILTERMODEL_H
//-----------------------------------------------------
#include <QtCore/QAbstractProxyModel>
//-----------------------------------------------------
class CFKHistoryFilterModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    CFKHistoryFilterModel(QAbstractItemModel* pSrcModel, QObject* pParent = NULL);
public:
    bool            IsHistoryContains(const QString& url) const;
public:
    QModelIndex     mapFromSource(const QModelIndex &sourceIndex) const;
    QModelIndex     mapToSource(const QModelIndex &proxyIndex) const;
    void            setSourceModel(QAbstractItemModel *sourceModel);
    QVariant        headerData(int nSection, Qt::Orientation eOrientation, int nRole = Qt::DisplayRole) const;
    int             rowCount(const QModelIndex &parent = QModelIndex()) const;
    int             columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex     index(int nRow, int nColumn, const QModelIndex& parent = QModelIndex()) const;
    QModelIndex     parent(const QModelIndex& index= QModelIndex()) const;
    bool            removeRows(int nRow, int nCount, const QModelIndex &parent = QModelIndex());
private slots:
    void            __SLOT_SourceReset();
    void            __SLOT_SourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void            __SLOT_SourceRowsInserted(const QModelIndex &parent, int nStart, int nEnd);
    void            __SLOT_SourceRowsRemoved(const QModelIndex &noUse, int nStart, int nEnd);
private:
    void            __Load() const;
private:
    mutable QList<int>          m_ListSourceRow;
    mutable QHash<QString, int> m_HistorysHash;
};
//-----------------------------------------------------
#endif // FKHISTORYFILTERMODEL_H
