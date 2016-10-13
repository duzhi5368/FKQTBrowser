/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKCOOKIEEXCEPTIONSMODEL_H
#define FKCOOKIEEXCEPTIONSMODEL_H
//-----------------------------------------------------
#include <QAbstractTableModel>
//-----------------------------------------------------
class CFKCookieJarMgr;
//-----------------------------------------------------
class CFKCookieExceptionsModel : public QAbstractTableModel
{
    friend class CFKCookieExceptionsDialog;
    Q_OBJECT
public:
    CFKCookieExceptionsModel(CFKCookieJarMgr* pMgr, QObject* pParent = NULL);
public:
    QVariant          headerData(int nSection,
                         Qt::Orientation eOrientation, int nRole) const;
    QVariant          data(const QModelIndex& tagIndex,
                              int nRole = Qt::DisplayRole) const;
    int               columnCount(const QModelIndex& parent = QModelIndex()) const;
    int               rowCount(const QModelIndex& parent = QModelIndex()) const;
    bool              removeRows(int nRow, int nCount, const QModelIndex& parent = QModelIndex());

private:
    CFKCookieJarMgr*            m_pCookieJarMgr;        // Cookie管理器
    QStringList                 m_ListAllowedCookies;   // 许可域列表
    QStringList                 m_ListBlockedCookies;   // 禁止域列表
    QStringList                 m_ListSessionCookies;   // 当前连接许可域列表
};
//-----------------------------------------------------
#endif // FKCOOKIEEXCEPTIONSMODEL_H
