/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include "fkhistoryitem.h"
//-----------------------------------------------------
CFKHistoryItem::CFKHistoryItem()
{

}
//-----------------------------------------------------
CFKHistoryItem::CFKHistoryItem(const QString& url, const QDateTime& dateTime,
               const QString &title)
    : m_strTitle(title)
    , m_strUrl(url)
    , m_tagDateTime(dateTime)
{

}
//-----------------------------------------------------
bool CFKHistoryItem::operator==(const CFKHistoryItem &other) const
{
    return other.m_strTitle == m_strTitle
            && other.m_strUrl == m_strUrl
            && other.m_tagDateTime == m_tagDateTime;
}
//-----------------------------------------------------
bool CFKHistoryItem::operator <(const CFKHistoryItem &other) const
{
    return m_tagDateTime > other.m_tagDateTime;
}
//-----------------------------------------------------
