/*
 * Author:Frankie.W
*/
//-----------------------------------------------------
#include <QtCore/QFile>
#include <QtCore/QIODevice>
//-----------------------------------------------------
#include "fkfwriter.h"
#include "fkbookmarknode.h"
//-----------------------------------------------------
CFKFWriter::CFKFWriter()
{
    setAutoFormatting(true);
}
//-----------------------------------------------------
bool CFKFWriter::Write(const QString& strFileName, const CFKBookmarkNode* pRoot)
{
    QFile file(strFileName);
    if (!pRoot || !file.open(QFile::WriteOnly))
        return false;
    return Write(&file, pRoot);
}
//-----------------------------------------------------
bool CFKFWriter::Write(QIODevice* pDevice, const CFKBookmarkNode* pRoot)
{
    setDevice(pDevice);

    writeStartDocument();
    writeDTD(QLatin1String("<!DOCTYPE FKFile>"));
    writeStartElement(QLatin1String("fkf"));
    writeAttribute(QLatin1String("version"), QLatin1String("1.0"));
    if (pRoot->GetType() == CFKBookmarkNode::eNT_Root)
    {
        for (int i = 0; i < pRoot->GetChildrens().count(); ++i)
        {
            __WriteItem(pRoot->GetChildrens().at(i));
        }
    }
    else
    {
        __WriteItem(pRoot);
    }

    writeEndDocument();
    return true;
}
//-----------------------------------------------------
void CFKFWriter::__WriteItem(const CFKBookmarkNode* pParent)
{
    switch (pParent->GetType())
    {
    case CFKBookmarkNode::eNT_Folder:
        writeStartElement(QLatin1String("folder"));
        writeAttribute(QLatin1String("folded"),
             pParent->m_bIsExpanded ? QLatin1String("no") : QLatin1String("yes"));
        writeTextElement(QLatin1String("title"), pParent->m_strTitle);
        for (int i = 0; i < pParent->GetChildrens().count(); ++i)
        {
            __WriteItem(pParent->GetChildrens().at(i));
        }
        writeEndElement();
        break;
    case CFKBookmarkNode::eNT_Bookmark:
        writeStartElement(QLatin1String("bookmark"));
        if (!pParent->m_strUrl.isEmpty())
        {
            writeAttribute(QLatin1String("href"), pParent->m_strUrl);
        }
        writeTextElement(QLatin1String("title"), pParent->m_strTitle);
        if (!pParent->m_strDesc.isEmpty())
        {
            writeAttribute(QLatin1String("desc"), pParent->m_strDesc);
        }
        writeEndElement();
        break;
    case CFKBookmarkNode::eNT_Separator:
        writeEmptyElement(QLatin1String("separator"));
        break;
    default:
        break;
    }
}
//-----------------------------------------------------
