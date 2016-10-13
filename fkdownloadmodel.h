/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKDOWNLOADMODEL_H
#define FKDOWNLOADMODEL_H
//-----------------------------------------------------
#include <QtCore/QAbstractItemModel>
//-----------------------------------------------------
class CFKDownloadDialog;
//-----------------------------------------------------
class CFKDownloadModel : public QAbstractListModel
{
    friend class CFKDownloadDialog;
    Q_OBJECT
public:
    CFKDownloadModel(CFKDownloadDialog* pDownloadDialog, QObject* pParent = NULL);
public:
    QVariant data(const QModelIndex &index, int nRole = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    bool removeRows(int nRow, int nCount, const QModelIndex &parent = QModelIndex());
private:
    CFKDownloadDialog*      m_pDownloadDialog;
};
//-----------------------------------------------------
#endif // FKDOWNLOADMODEL_H
