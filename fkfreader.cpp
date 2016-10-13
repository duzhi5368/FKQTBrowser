/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtCore/QFile>
#include <QtCore/QIODevice>
//-----------------------------------------------------
#include "fkfreader.h"
#include "fkbookmarknode.h"
//-----------------------------------------------------
CFKFReader::CFKFReader()
{

}
//-----------------------------------------------------
CFKBookmarkNode* CFKFReader::Read(const QString& strFileName)
{
    QFile file(strFileName);
    if (!file.exists())
    {
        return new CFKBookmarkNode(CFKBookmarkNode::eNT_Root);
    }
    file.open(QFile::ReadOnly);
    return Read(&file);
}
//-----------------------------------------------------
CFKBookmarkNode* CFKFReader::Read(QIODevice* pDevice)
{
    CFKBookmarkNode *pRoot = new CFKBookmarkNode(CFKBookmarkNode::eNT_Root);
    setDevice(pDevice);
    if (readNextStartElement())
    {
        QString strVersion = attributes().value(QLatin1String("version")).toString();
        if (name() == QLatin1String("fkf")
            && (strVersion.isEmpty() || strVersion == QLatin1String("1.0")))
        {
            __ReadFKF(pRoot);
        }
        else
        {
            raiseError(QObject::tr("The file is not an FKF version 1.0 file."));
        }
    }
    return pRoot;
}
//-----------------------------------------------------
void CFKFReader::__ReadFKF(CFKBookmarkNode* pParent)
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("fkf"));

    while (readNextStartElement())
    {
        if (name() == QLatin1String("folder"))
            __ReadFolder(pParent);
        else if (name() == QLatin1String("bookmark"))
            __ReadBookmarkNode(pParent);
        else if (name() == QLatin1String("separator"))
            __ReadSeparator(pParent);
        else
            skipCurrentElement();
    }
}
//-----------------------------------------------------
void CFKFReader::__ReadTitle(CFKBookmarkNode* pParent)
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("title"));
    pParent->m_strTitle = readElementText();
}
//-----------------------------------------------------
void CFKFReader::__ReadDescription(CFKBookmarkNode* pParent)
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("desc"));
    pParent->m_strDesc = readElementText();
}
//-----------------------------------------------------
void CFKFReader::__ReadSeparator(CFKBookmarkNode* pParent)
{
    new CFKBookmarkNode(CFKBookmarkNode::eNT_Separator, pParent);
    readNext();
}
//-----------------------------------------------------
void CFKFReader::__ReadFolder(CFKBookmarkNode* pParent)
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("folder"));

    CFKBookmarkNode *pFolder = new CFKBookmarkNode(CFKBookmarkNode::eNT_Folder, pParent);
    pFolder->m_bIsExpanded = (attributes().value(QLatin1String("folded")) == QLatin1String("no"));

    while (readNextStartElement())
    {
        if (name() == QLatin1String("title"))
            __ReadTitle(pFolder);
        else if (name() == QLatin1String("desc"))
            __ReadDescription(pFolder);
        else if (name() == QLatin1String("folder"))
            __ReadFolder(pFolder);
        else if (name() == QLatin1String("bookmark"))
            __ReadBookmarkNode(pFolder);
        else if (name() == QLatin1String("separator"))
            __ReadSeparator(pFolder);
        else
            skipCurrentElement();
    }
}
//-----------------------------------------------------
void CFKFReader::__ReadBookmarkNode(CFKBookmarkNode* pParent)
{
    Q_ASSERT(isStartElement() && name() == QLatin1String("bookmark"));

    CFKBookmarkNode* pBookmark = new CFKBookmarkNode(CFKBookmarkNode::eNT_Bookmark, pParent);
    pBookmark->m_strUrl = attributes().value(QLatin1String("href")).toString();
    while (readNextStartElement())
    {
        if (name() == QLatin1String("title"))
            __ReadTitle(pBookmark);
        else if (name() == QLatin1String("desc"))
            __ReadDescription(pBookmark);
        else
            skipCurrentElement();
    }
    if (pBookmark->m_strTitle.isEmpty())
    {
        pBookmark->m_strTitle = QObject::tr("Unknown title");
    }
}
//-----------------------------------------------------
