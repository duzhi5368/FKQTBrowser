/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtGui/QFont>
#include <QtGui/QFontMetrics>
//-----------------------------------------------------
#include "fkcookieexceptionsmodel.h"
#include "fkcookiejarmgr.h"
//-----------------------------------------------------
CFKCookieExceptionsModel::CFKCookieExceptionsModel(CFKCookieJarMgr* pMgr, QObject* pParent)
    : QAbstractTableModel(pParent)
    , m_pCookieJarMgr(pMgr)
{
    m_ListAllowedCookies = m_pCookieJarMgr->GetAllowedCookies();
    m_ListBlockedCookies = m_pCookieJarMgr->GetBlockedCookies();
    m_ListSessionCookies = m_pCookieJarMgr->GetAllowForSessionCookies();
}
//-----------------------------------------------------
QVariant CFKCookieExceptionsModel::headerData(int nSection,
                     Qt::Orientation eOrientation, int nRole) const
{
    if (nRole == Qt::SizeHintRole)
    {
        QFont tagFont;
        tagFont.setPointSize(10);
        QFontMetrics tagFontMetrics(tagFont);
        int nHeight = tagFontMetrics.height() + tagFontMetrics.height()/3;
        int nWidth = tagFontMetrics.width(headerData(nSection, eOrientation, Qt::DisplayRole).toString());
        return QSize(nWidth, nHeight);
    }

    if (eOrientation == Qt::Horizontal
        && nRole == Qt::DisplayRole)
    {
        switch (nSection)
        {
            case 0:
                return tr("Website");
            case 1:
                return tr("Status");
        }
    }
    return QAbstractTableModel::headerData(nSection, eOrientation, nRole);
}
//-----------------------------------------------------
QVariant CFKCookieExceptionsModel::data(const QModelIndex& tagIndex, int nRole) const
{
    if (tagIndex.row() < 0 || tagIndex.row() >= rowCount())
    {
        return QVariant();
    }

    switch (nRole)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        {
            int nRow = tagIndex.row();
            if (nRow < m_ListAllowedCookies.count())
            {
                switch (tagIndex.column())
                {
                    case 0:
                        return m_ListAllowedCookies.at(nRow);
                    case 1:
                        return tr("Allow");
                }
            }
            nRow = nRow - m_ListAllowedCookies.count();
            if (nRow < m_ListBlockedCookies.count())
            {
                switch (tagIndex.column())
                {
                    case 0:
                        return m_ListBlockedCookies.at(nRow);
                    case 1:
                        return tr("Block");
                }
            }
            nRow = nRow - m_ListBlockedCookies.count();
            if (nRow < m_ListSessionCookies.count())
            {
                switch (tagIndex.column())
                {
                    case 0:
                        return m_ListSessionCookies.at(nRow);
                    case 1:
                        return tr("Allow For Session");
                }
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
int CFKCookieExceptionsModel::columnCount(const QModelIndex& parent) const
{
    return (parent.isValid()) ? 0 : 2;
}
//-----------------------------------------------------
int CFKCookieExceptionsModel::rowCount(const QModelIndex& parent) const
{
return (parent.isValid() || !m_pCookieJarMgr) ?
            0 : m_ListAllowedCookies.count() + m_ListBlockedCookies.count() + m_ListSessionCookies.count();
}
//-----------------------------------------------------
bool CFKCookieExceptionsModel::removeRows(int nRow, int nCount,
                                          const QModelIndex& parent)
{
    if (parent.isValid() || !m_pCookieJarMgr)
    {
        return false;
    }

    int nLastRow = nRow + nCount - 1;

    beginRemoveRows(parent, nRow, nLastRow);
    for (int i = nLastRow; i >= nRow; --i)
    {
        if (i < m_ListAllowedCookies.count()) {
            m_ListAllowedCookies.removeAt(nRow);
            continue;
        }
        i = i - m_ListAllowedCookies.count();
        if (i < m_ListBlockedCookies.count()) {
            m_ListBlockedCookies.removeAt(nRow);
            continue;
        }
        i = i - m_ListBlockedCookies.count();
        if (i < m_ListSessionCookies.count()) {
            m_ListSessionCookies.removeAt(nRow);
            continue;
        }
    }
    m_pCookieJarMgr->SetAllowedCookies(m_ListAllowedCookies);
    m_pCookieJarMgr->SetBlockedCookies(m_ListBlockedCookies);
    m_pCookieJarMgr->SetAllowForSessionCookies(m_ListSessionCookies);
    endRemoveRows();

    return true;
}
//-----------------------------------------------------
