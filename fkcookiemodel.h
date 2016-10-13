/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKCOOKIEMODEL_H
#define FKCOOKIEMODEL_H
//-----------------------------------------------------
#include <QtCore/QAbstractItemModel>
//-----------------------------------------------------
class CFKCookieJarMgr;
//-----------------------------------------------------
class CFKCookieModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    CFKCookieModel(CFKCookieJarMgr* pMgr, QObject* pParent = NULL);
public:
    QVariant          headerData(int nSection,
                         Qt::Orientation eOrientation, int nRole) const;
    QVariant          data(const QModelIndex& tagIndex,
                              int nRole = Qt::DisplayRole) const;
    int               columnCount(const QModelIndex& parent = QModelIndex()) const;
    int               rowCount(const QModelIndex& parent = QModelIndex()) const;
    bool              removeRows(int nRow, int nCount, const QModelIndex& parent = QModelIndex());
private slots:
    void              SLOT_CookiesChanged();
private:
    CFKCookieJarMgr*  m_pCookieJarMgr;
};
//-----------------------------------------------------
#endif // FKCOOKIEMODEL_H
