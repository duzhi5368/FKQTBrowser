/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKHISTORYCOMPLETIONMODEL_H
#define FKHISTORYCOMPLETIONMODEL_H
//-----------------------------------------------------
#include <QtCore/QAbstractProxyModel>
//-----------------------------------------------------
class CFKHistoryCompletionModel : public QAbstractProxyModel
{
    Q_OBJECT
public:
    CFKHistoryCompletionModel(QObject* pParent = NULL);
public:
    QVariant    data(const QModelIndex &index, int nRole = Qt::DisplayRole) const;
    int         rowCount(const QModelIndex &parent = QModelIndex()) const;
    int         columnCount(const QModelIndex &parent = QModelIndex()) const;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;
    QModelIndex index(int nRow, int nColumn, const QModelIndex& parent= QModelIndex()) const;
    QModelIndex parent(const QModelIndex& index = QModelIndex()) const;
    void        setSourceModel(QAbstractItemModel* pSourceModel);
private slots:
    void        __SLOT_SourceReset();
};
//-----------------------------------------------------
#endif // FKHISTORYCOMPLETIONMODEL_H
