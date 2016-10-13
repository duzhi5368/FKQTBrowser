/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>

#include <QtNetwork/QNetworkCookie>

#include <QtCore>
//-----------------------------------------------------
#include "fkcookiemodel.h"
#include "fkcookiejarmgr.h"
//-----------------------------------------------------
CFKCookieModel::CFKCookieModel(CFKCookieJarMgr* pMgr, QObject* pParent)
    : QAbstractTableModel(pParent)
    , m_pCookieJarMgr(pMgr)
{
    // 函数衔接
    connect( m_pCookieJarMgr, SIGNAL(SIGNAL_CookiesChanged()),
             this, SLOT(SLOT_CookiesChanged()));
    m_pCookieJarMgr->__LoadCookies();
}
//-----------------------------------------------------
QVariant CFKCookieModel::headerData(int nSection,
                     Qt::Orientation eOrientation, int nRole) const
{
    if(nRole == Qt::SizeHintRole)
    {
        QFont font;
        font.setPointSize(10);
        QFontMetrics fontMetrics(font);
        int nHeight = fontMetrics.height() + fontMetrics.height() / 3;
        int nWidth  = fontMetrics.width(headerData(nSection, eOrientation, Qt::DisplayRole).toString());
        return QSize(nWidth, nHeight);
    }

    if(eOrientation == Qt::Horizontal)
    {
        if(nRole != Qt::DisplayRole)
            return QVariant();

        switch(nSection)
        {
        case 0:
            return tr("Website");
        case 1:
            return tr("Name");
        case 2:
            return tr("Path");
        case 3:
            return tr("Secure");
        case 4:
            return tr("Expires");
        case 5:
            return tr("Contents");
        default:
            return QVariant();
        }
    }
    return QAbstractTableModel::headerData(nSection, eOrientation, nRole);
}
//-----------------------------------------------------
QVariant CFKCookieModel::data(const QModelIndex& tagIndex,int nRole) const
{
    QList<QNetworkCookie> ListCookies;
    if (m_pCookieJarMgr)
        ListCookies = m_pCookieJarMgr->allCookies();

    if (tagIndex.row() < 0 || tagIndex.row() >= ListCookies.size())
    {
        return QVariant();
    }

    switch (nRole)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        {
            QNetworkCookie tagCookie = ListCookies.at(tagIndex.row());
            switch (tagIndex.column())
            {
                case 0:
                    return tagCookie.domain();
                case 1:
                    return tagCookie.name();
                case 2:
                    return tagCookie.path();
                case 3:
                    return tagCookie.isSecure();
                case 4:
                    return tagCookie.expirationDate();
                case 5:
                    return tagCookie.value();
            }
        }
        break;
    case Qt::FontRole:
        {
            QFont font;
            font.setPointSize(10);
            return font;
        }
        break;
    }

    return QVariant();
}
//-----------------------------------------------------
int CFKCookieModel::columnCount(const QModelIndex& parent) const
{
    return (parent.isValid()) ? 0 : 6;
}
//-----------------------------------------------------
int CFKCookieModel::rowCount(const QModelIndex& parent) const
{
    return (parent.isValid() || !m_pCookieJarMgr) ? 0 : m_pCookieJarMgr->allCookies().count();
}
//-----------------------------------------------------
bool CFKCookieModel::removeRows(int nRow, int nCount, const QModelIndex& parent)
{
    if (parent.isValid() || !m_pCookieJarMgr)
        return false;

    int nLastRow = nRow + nCount - 1;
    beginRemoveRows(parent, nRow, nLastRow);
    QList<QNetworkCookie> ListCookies = m_pCookieJarMgr->allCookies();
    for (int i = nLastRow; i >= nRow; --i)
    {
        ListCookies.removeAt(i);
    }
    m_pCookieJarMgr->setAllCookies(ListCookies);
    endRemoveRows();
    return true;
}
//-----------------------------------------------------
void CFKCookieModel::SLOT_CookiesChanged()
{
    beginResetModel();
    endResetModel();
}
//-----------------------------------------------------
