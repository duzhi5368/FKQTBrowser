/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#ifndef FKHISTORYITEM_H
#define FKHISTORYITEM_H
//-----------------------------------------------------
#include <QtCore/QDateTime>
//-----------------------------------------------------
class CFKHistoryItem
{
    friend class CFKHistoryModel;
    friend class CFKHistoryMgr;
public:
    CFKHistoryItem();
    CFKHistoryItem(const QString& url, const QDateTime& dateTime = QDateTime(),
                   const QString &title = QString());
public:
    bool operator==(const CFKHistoryItem &other) const;
    bool operator <(const CFKHistoryItem &other) const;
private:
    QString     m_strTitle;
    QString     m_strUrl;
    QDateTime   m_tagDateTime;
};
//-----------------------------------------------------
#endif // FKHISTORYITEM_H
